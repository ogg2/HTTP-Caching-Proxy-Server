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

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"


void process_request(ServerClient & server, int fd, std::set<int> & ids, Cache * cache) {
  write_log(fd, "Connection opened");
  
  Request * request = server.receive_request(fd);

  if (request == nullptr) {
    std::cerr << "Empty request" << std::endl;
    write_log(fd, "Received empty request");
    return;
  }

  CacheEntry * entry;
  if (request->get_type() == GET) {
    std::string url = request->get_url();
    entry = cache->find_response(url);
    if (entry != nullptr) {
      std::vector<char> resp = entry->get_response()->make_response();
      server.send_response(resp, fd);
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

  Response * response = client.client_receive();
  if (request->get_type() == GET) {
    bool no_store = false;
    unordered_map<string, int> cache_directives = response->get_cache_control();
    if (cache_directives.empty()) {
      entry = new CacheEntry(response, 0, false);
    } else {
      int max_age;
      bool revalidate = false;
      unordered_map<string, int>::iterator it = cache_directives.find("max-age");
      if (it != unordered_map<string, int>::end) {
        max_age = it->second;
      }
      it = cache_directives.find("no-cache");
      if (it != unordered_map<string, int>::end) {
        revalidate = true;
      }
      it = cache_directives.find("no-store");
      if (it != unordered_map<string, int>::end) {
        no_store = true;
      }
      entry = new CacheEntry(response, max_age, revalidate);
    }
    if (!no_store) {
      cache->add_entry(request->get_url(), entry);
      std::cout << "CACHING NEW ENTRY" << std::endl;
    }
  }
  
  client.close_socket();

  if (response == nullptr) {
    std::cerr << "Error response" << std::endl;
    return;
  }

  std::vector<char> resp = response->make_response();
  server.send_response(resp, fd);
  write_log(fd, "Sent response from origin server");

  ids.erase(fd);
}
