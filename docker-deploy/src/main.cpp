#include <pthread.h>
#include <cstdio>

#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>

#include "serverClient.hpp"
#include "response.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"

/* 
- Socket daemon module to wait and recieve connections from clients
  - Each connection spawns a new thread to run proxy server module
*/

void proccess_request(ServerClient server, int fd) {
  std::cout << "starting thread...\n"; 
  Request * request = server.receive_request(fd);
  std::cout << "1. reccieved request\n";
   
  bool is_server = false;
  const char * hostname = request->get_hostname();
  const char * port = "80";
  
  ServerClient client(hostname, port);
  int status = client.initialize_socket(is_server);
  if (status == -1) {
    std::cout << "Failed to setup client" << std::endl;
    return;
  }

  std::cout << "2. created socket\n"; 
  if (!client.send_request(request->make_get_req())) { return; }

  std::cout << "3. sent request\n"; 
  Response * response = client.client_receive();

  std::cout << "4. recieved response:\n";
  client.close_socket();
  
  //response->print();

  std::vector<char> resp = response->make_response();
  //std::cout << string(resp.begin(), resp.end()) << std::endl;
  server.send_response(resp, fd);
  std::cout << "5. sent response:\n"; 
}


//void server_message(ServerClient server, int client_id) {
//Response * r = server.receive_message(client_id);
//}

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
      std::thread (proccess_request, std::ref(server), client_id).detach(); //do we need to use std::ref?
    }
    //process request
    //send response
  }
  return EXIT_SUCCESS;
}
