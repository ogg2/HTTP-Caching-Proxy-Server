#include <pthread.h>
#include <cstdio>

#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <set>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include "serverClient.hpp"
#include "request.hpp"
#include "response.hpp"
#include "log.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"


void process_request(ServerClient & server, int fd, std::set<int> & ids, Cache * cache, boost::mutex& log_mu) {
  
  Request * request = server.receive_request(fd);

  if (request == nullptr) {
    // create response for malformed request
    log_request(fd, "ERROR Empty request", log_mu);
    return;
  }

  string request_line = '\"' + request->get_request_line() + '\"';
  log_request(fd, request_line, log_mu);

  CacheEntry * entry;
  if (request->get_type() == GET) {
    std::string url = request->get_url();
    entry = cache->find_response(url);
    if (entry != nullptr) {
      std::vector<char> resp = entry->get_response()->make_response();
      server.send_response(resp, fd);
      log_phrase(fd, "in cache, valid", log_mu);
      return;
    }
    log_phrase(fd, "not in cache", log_mu);
  }
   
  bool is_server = false;
  const char * hostname = request->get_hostname();
  const char * port = request->get_port();
  
  ServerClient client(hostname, port);
  int status = client.initialize_socket(is_server);
  if (status == -1) {
    log_phrase(fd, "ERROR Failed to setup client", log_mu);
    client.close_socket();
    return;
  }

  if (request->get_type() == CONNECT) {
    client.connect_tunnel(fd);
    client.close_socket();
    log_phrase(fd, "Tunnel closed", log_mu);
    return;
  } else if (!client.send_request(request->make_request())) {
    client.close_socket();
    return;
  }
  
  log_origin_request(fd, request->get_request_line(), request->get_hostname(), log_mu);

  Response * response = client.client_receive();

  log_origin_response(fd, response->get_response_line(), request->get_hostname(), log_mu);
  
  if ((request->get_type() == GET) && (response->get_status() == 200)) {
    bool no_store = false;
    unordered_map<string, int> cache_directives = response->get_cache_control();
    if (cache_directives.empty()) {
      entry = new CacheEntry(response, 0, false);
      log_phrase(fd, "cached", log_mu);
    } else {
      int max_age = 0;
      bool revalidate = false;
      unordered_map<string, int>::iterator it = cache_directives.find("max-age");
      if (it != cache_directives.end()) {
        max_age = it->second;
      }
      it = cache_directives.find("no-cache");
      if (it != cache_directives.end()) {
        revalidate = true;
      }
      it = cache_directives.find("no-store");
      if (it != cache_directives.end()) {
        no_store = true;
      }
      entry = new CacheEntry(response, max_age, revalidate);
    }
    if (!no_store) {
      cache->add_entry(request->get_url(), entry);
    }
  }
  
  client.close_socket();

  if (response == nullptr) {
    std::cerr << "Error response" << std::endl;
    // make error response
    return;
  }

  std::vector<char> resp = response->make_response();
  server.send_response(resp, fd);
  log_response(fd, response->get_response_line(), log_mu);

  ids.erase(fd);
}
