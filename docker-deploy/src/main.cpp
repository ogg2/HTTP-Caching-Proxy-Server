#include <pthread.h>
#include <thread>
#include <sstream>
#include <mutex>
#include <iostream>
#include <stdio>

#include "server.hpp"

#define LOGGING "Start Logging my task = %d\n \tThread ID: %s\n"
#define THREADLOG "\tThread ID = %s\n"

/* 
- Socket daemon module to wait and recieve connections from clients
  - Each connection spawns a new thread to run proxy server module



*/

//std::mutex test_mutex;

char * make_GET_request(char * path, char * hostaddr) {
  char p_request[100]={0};
 
  char p_resourcepath[]="2019/07/creating-xml-request-in-cpp-for-server.html";
 
  char p_hostaddress[]="www.cplusplus.com";
 
  sprintf(p_request, "GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: text/plain\r\n\r\n", p_resourcepath, p_hostaddress);

  return p_request;
}


int make_GET_socket(const char * hostname) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *port     = "8080";

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  char * resource = "resource";

  char * message = make_GET_request(resource, hostname);
  
  send(socket_fd, message, strlen(message), 0);

  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;

}

void recieve_requests(Server& server, int client_id) {

  ssize_t len = 1;
  char buffer [512];
  //std::lock_guard<std::mutex> guard(test_mutex);


  while (len != 0) {
    len = recv(client_id, buffer, 512, 0);
    if (buffer == "GET") {
      make_GET_socket();
    }
  }

  if (

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
      std::thread (count_up, std::ref(s), client_id).detach();
      //syslog(LOG_INFO, LOGGING, count++, oss.str().c_str());
    }
    //process request
    //send response
  }
  //  closelog();
  return EXIT_SUCCESS;
}
