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
#include "log.hpp"

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
  clear_log();
  boost::mutex log_mu;
  
  ServerClient server("0.0.0.0", "12345");
  bool is_server = true;
  int status = server.initialize_socket(is_server);
  if (status == EXIT_FAILURE) {
    log_phrase(0, "ERROR Failed to setup server", log_mu);
    return EXIT_FAILURE;
  }
  
  Cache * cache = new Cache;;
  while (1) {
    int client_fd = server.accept_connections();
    if (client_fd == -1) {
      log_phrase(0, "ERROR Cannot accept connection on socket", log_mu);
    } else {
      std::thread(process_request, std::ref(server), client_fd, cache, std::ref(log_mu)).detach();
      //boost::thread(boost::bind(process_request, std::ref(server), client_fd, std::ref(ids), cache, std::ref(log_mu))).detach();
    }
  }

  delete cache;
  server.close_socket();
  return EXIT_SUCCESS;
}
