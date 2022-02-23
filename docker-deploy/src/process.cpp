#include <pthread.h>
#include <cstdio>

#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <set>

#include "serverClient.hpp"
#include "request.hpp"
#include "response.hpp"
#include "log.hpp"
#include "process.hpp"

void process_request(ServerClient & server, int fd, Cache * cache, std::mutex& log_mu) {
  
  Request * request;
  Response * response;
  
  request = server.receive_request(fd);

  if (request == nullptr) {
    log_request(fd, "ERROR Empty request", log_mu);
    response = make_400_response();
    server.send_response(response->make_response(), fd);
    log_response(fd, response->get_response_line(), log_mu);
    delete response;
    return;
  }

  log_request(fd, request->get_request_line(), log_mu);

  CacheEntry * entry;
  std::string url = request->get_url();
  if (request->get_type() == GET) {
    entry = cache->find_response(url);
    if (entry != nullptr) {
      if (entry->is_fresh()) {
        std::vector<char> resp = entry->get_response()->make_response();
        server.send_response(resp, fd);
        log_phrase(fd, "in cache, valid", log_mu);
	      delete request;
        return;
      } else if (!entry->needs_revalidation()) {
        std::string expiration = entry->get_expiration();
        cache->remove_entry(url);
        log_phrase(fd, "in cache, but expired at " + expiration, log_mu);
      } else { //needs revalidation
        request->add_header("If-Modified-Since", entry->get_expiration());
        log_phrase(fd, "in cache, requires validation", log_mu);
      }
    } else {
      log_phrase(fd, "not in cache", log_mu);
    }
  }
   
  bool is_server = false;
  const char * hostname = request->get_hostname();
  const char * port = request->get_port();
  
  ServerClient client(hostname, port);
  int status = client.initialize_socket(is_server);
  if (status == -1) {
    log_phrase(fd, "ERROR Failed to setup client", log_mu);
    response = make_502_response();
    server.send_response(response->make_response(), fd);
    log_response(fd, response->get_response_line(), log_mu);
    client.close_socket();
    delete response;
    delete request;
    return;
  }

  //CONNECT 
  if (request->get_type() == CONNECT) {
    client.connect_tunnel(fd);
    client.close_socket();
    log_phrase(fd, "Tunnel closed", log_mu);
    delete request;
    return;
  } else if (!client.send_request(request->make_request())) {
    response = make_502_response();
    server.send_response(response->make_response(), fd);
    log_response(fd, response->get_response_line(), log_mu);
    client.close_socket();
    //delete response;
    //delete request;
    return;
  }
  
  log_origin_request(fd, request->get_request_line(), request->get_hostname(), log_mu);

  response = client.client_receive();

  //304: not modifed after validation request
  if (response->get_status() == 304) { 
    response = entry->get_response();
    log_phrase(fd, "NOTE revalidated", log_mu); 
  }

  log_origin_response(fd, response->get_response_line(), request->get_hostname(), log_mu);
  
  if ((request->get_type() == GET) && (response->get_status() == 200)) {
    bool no_store = false;
    unordered_map<string, int> cache_directives = response->get_cache_control();
    if (cache_directives.empty()) {
      entry = new CacheEntry(response, 0, false, true);
      log_phrase(fd, "cached", log_mu);
    } else {
      int max_age = cache_max_age(std::ref(cache_directives), fd);
      bool revalidate = cache_revalidate(std::ref(cache_directives), fd, std::ref(log_mu));
      no_store = cache_no_store(std::ref(cache_directives), fd, std::ref(log_mu));

      entry = new CacheEntry(response, max_age, revalidate, false);
      if (max_age != 0) {
        log_phrase(fd, "cached, expires at " + entry->get_expiration(), log_mu);
      }
    }
    if (!no_store) {
      cache->add_entry(request->get_url(), entry);
    }
  }
  
  client.close_socket();

  if (response == nullptr) {
    std::cerr << "Error response" << std::endl;
    response = make_502_response();
    server.send_response(response->make_response(), fd);
    log_response(fd, response->get_response_line(), log_mu);
    //delete response;
    //delete request;
    return;
  }

  std::vector<char> resp = response->make_response();
  server.send_response(resp, fd);
  log_response(fd, response->get_response_line(), log_mu);

  //delete response;
  //delete request;
}

bool cache_revalidate(unordered_map<string, int> & directives, int fd, std::mutex& log_mu) {
  unordered_map<string, int>::iterator it = directives.find("no-cache");
  if (it != directives.end()) {
    log_phrase(fd, "cached, but requires re-validation", log_mu);
    return true;
  }
  it = directives.find("must-revalidate");
  if (it != directives.end()) {
    log_phrase(fd, "cached, but requires re-validation", log_mu);
    return true;
  }
  it = directives.find("max-age");
  if (it != directives.end() && it->second == 0) {
    log_phrase(fd, "cached, but requires re-validation", log_mu);
    return true;
  }
  return false;
}

int cache_max_age(unordered_map<string, int> & directives, int fd) {
  unordered_map<string, int>::iterator it = directives.find("max-age");
  if (it != directives.end()) {
    return it->second;
  }
  it = directives.find("s-maxage");
  if (it != directives.end()) {
    return it->second;
  }
  return 0;
}

bool cache_no_store(unordered_map<string, int> & directives, int fd, std::mutex& log_mu) {
  unordered_map<string, int>::iterator it = directives.find("no-store");
  if (it != directives.end()) {
    log_phrase(fd, "not cacheable, because no-store", log_mu);
    return true;
  }
  it = directives.find("private");
  if (it != directives.end()) {
    log_phrase(fd, "not cacheable, because private", log_mu);
    return true;
  }
  return false;
}

Response * make_502_response() {
  return new Response(502, "Bad Gateway", std::map<string, string>(), std::vector<char>());
}

Response * make_400_response() {
  return new Response(400, "Bad Request", std::map<string, string>(), std::vector<char>());
}
