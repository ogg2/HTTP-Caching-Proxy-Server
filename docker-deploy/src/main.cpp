#include <pthread.h>
#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>

#include "server.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"

/* 
- Socket daemon module to wait and recieve connections from clients
  - Each connection spawns a new thread to run proxy server module



*/

std::mutex test_mutex;

void count_up(int& x, int client_id) {
  char buffer[512];
  recv(client_id, buffer, 9, 0);
  buffer[9] = 0;

  std::cout << "Server received: " << buffer << std::endl;
  
  std::lock_guard<std::mutex> guard(test_mutex);
  x++;
  std::ostringstream oss;
  std::cout << std::this_thread::get_id();
}


int main () { 
  Server s;
  int ret = s.server_init();
  if (ret == EXIT_FAILURE) {
    std::cout << "Failed to initialize server." << std::endl;
    exit(EXIT_FAILURE);
  }
  int count = 0;
  while (1) {
    sleep(2);
    int client_id = s.accept_connections();
    if (client_id == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
    } else {
      std::thread (count_up, std::ref(count), client_id).detach();
      //syslog(LOG_INFO, LOGGING, count++, oss.str().c_str());
    }
    //process request
    //send response
  }
  //  closelog();
  return EXIT_SUCCESS;
}
