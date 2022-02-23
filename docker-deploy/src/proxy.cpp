#include <pthread.h>
#include <cstdio>

#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <set>
//#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "serverClient.hpp"
#include "process.hpp"
#include "cache.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"

/* 
- Socket daemon module to wait and recieve connections from clients
  - Each connection spawns a new thread to run proxy server module
*/

//void server_message(ServerClient server, int client_id) {
//Response * r = server.receive_message(client_id);
//}

int main () { 
  ServerClient server("0.0.0.0", "12345");
  bool is_server = true;
  int status = server.initialize_socket(is_server);
  if (status == EXIT_FAILURE) {
    std::cout << "Failed to setup server." << std::endl;
    return EXIT_FAILURE;
  }
  
  std::set<int> ids;

  boost::mutex log_mu;
  
  Cache * cache = new Cache;;
  while (1) {
    int client_fd = server.accept_connections();
    //std::cout << "fd: " << client_id << std::endl;
    if (client_fd == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
    } else if (ids.count(client_fd) == 0) {
      ids.insert(client_fd);
      std::thread(process_request, std::ref(server), client_fd, std::ref(ids), cache, std::ref(log_mu)).detach();
      //boost::thread(boost::bind(process_request, std::ref(server), client_fd, std::ref(ids), cache, std::ref(log_mu))).detach();
    }
  }

  delete cache;
  server.close_socket();
  return EXIT_SUCCESS;
}
