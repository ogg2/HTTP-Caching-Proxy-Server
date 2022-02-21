#include <pthread.h>
#include <cstdio>

#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <set>

#include "serverClient.hpp"
#include "process.hpp"

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
  while (1) {
    int client_id = server.accept_connections();
    //std::cout << "fd: " << client_id << std::endl;
    if (client_id == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
    } else if (ids.count(client_id) == 0) {
      ids.insert(client_id);
      std::thread(proccess_request, std::ref(server), client_id, std::ref(ids)).detach(); //do we need to use std::ref?
    }
  }

  server.close_socket();
  return EXIT_SUCCESS;
}
