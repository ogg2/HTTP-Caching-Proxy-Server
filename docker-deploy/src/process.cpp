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
    std::cerr << "Empty request" << std::endl;
    //log_request(fd, "Received empty request", log_mu);
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
      //write_log(fd, "Received empty request", log_mu);
      std::cout << "CACHED" << std::endl;
      return;
    }
  }
   
  bool is_server = false;
  const char * hostname = request->get_hostname();
  const char * port = request->get_port();
  
  ServerClient client(hostname, port);
  int status = client.initialize_socket(is_server);
  if (status == -1) {
    std::cout << "Failed to setup client" << std::endl;
    client.close_socket();
    return;
  }

  if (request->get_type() == CONNECT) {
    client.connect_tunnel(fd);
    client.close_socket();
    return;
  } else if (!client.send_request(request->make_request())) {
    client.close_socket();
    return;
  } 

  //CacheEntry * cachedResponse;
  Response * response;
  //if ((cachedResponse = server.get_cache()->find_response(request->get_resource())) != nullptr) {
  //  std::cout << "------Cached Response------" << std::endl;
  //  response = cachedResponse->get_response();
  //TODO check if cache entry is expired
  //} else {
  response = client.client_receive();
  //}
  if ((request->get_type() == GET) && (response->get_status() == 200)) {
    //if (request->get_cache_control() == XXXXXXX)
    entry = new CacheEntry(response, 0);
    cache->add_entry(request->get_url(), entry);
    std::cout << "CACHING NEW ENTRY" << std::endl;
  }
  
  client.close_socket();

  if (response == nullptr) {
    std::cerr << "Error response" << std::endl;
    return;
  }

  std::vector<char> resp = response->make_response();
  //TODO CacheEntry entry(response, MAX_AGE); create cache entry
  //TODO server.get_cache()->add_entry(RESOURCEURL, &entry);
  server.send_response(resp, fd);
  //write_log(fd, "Sent response from origin server", log_mu);

  ids.erase(fd);
}
