#include <pthread.h>
#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <cstdio>

#include "serverClient.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"

/* 
- Socket daemon module to wait and recieve connections from clients
  - Each connection spawns a new thread to run proxy server module
*/

void server_message(ServerClient server, int client_id) {
  server.receive_message(client_id);
}

int main () { 
  ServerClient server("0.0.0.0", "12345");
  bool is_server = true;
  int status = server.initialize_socket(is_server);
  if (status == EXIT_FAILURE) {
    std::cout << "Failed to setup server." << std::endl;
    exit(EXIT_FAILURE);
  }
  while (1) {
    int client_id = server.accept_connections();
    if (client_id == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
    } else {
      std::thread (server_message, std::ref(server), client_id).detach(); //do we need to use std::ref?
    }
    //process request
    //send response
  }
  return EXIT_SUCCESS;
}
