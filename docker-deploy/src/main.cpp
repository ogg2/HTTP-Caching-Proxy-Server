#include <pthread.h>
#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>

#include "server.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"

/* 
- Socket daemon module to wait and recieve connections from clients
  - Each connection spawns a new thread to run proxy server module



*/

std::mutex test_mutex;

void count_up(Server& server, int& x, int client_id) {
  
  std::lock_guard<std::mutex> guard(test_mutex);
  x++;

  server.receive_message(client_id);

  //syslog(LOG_INFO, THREADLOG, oss.str().c_str());
  std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
  std::cout << "X: " << x << std::endl;
  sleep(10);
  
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
      std::thread (count_up, std::ref(s), std::ref(count), client_id).detach();
      //syslog(LOG_INFO, LOGGING, count++, oss.str().c_str());
    }
    //process request
    //send response
  }
  //  closelog();
  return EXIT_SUCCESS;
}
