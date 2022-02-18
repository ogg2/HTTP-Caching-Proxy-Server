#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <string>
#include "responses.hpp"
#include "parse_responses.hpp"

class ServerClient {
private:
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  const char * hostname;
  const char * port;

public:
  ServerClient(const char * hostname_, const char * port_) : hostname(hostname_), port(port_) {
    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;
  }

  int initialize_socket(bool is_server) {
    if (is_server) {
      if (get_address_info() && create_socket() && bind_socket() && listen_socket()) {
        std::cout << "Ready for connection on port " << port << std::endl;
        return EXIT_SUCCESS;
      }
    } else {
      if (get_address_info() && create_socket() && connect_socket()) {
        std::cout << "Connected to " << hostname << " on port " << port << std::endl;
        return EXIT_SUCCESS;
      }
    }
    return EXIT_FAILURE;
  }

  bool get_address_info() {
    int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
      std::cerr << "Error: cannot get address info for host" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool create_socket() {
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
      std::cerr << "Error: cannot create socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool bind_socket() {
    int yes = 1;
    int status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cerr << "Error: cannot bind socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool listen_socket() {
    int status = listen(socket_fd, 100);
    if (status == -1) {
      std::cerr << "Error: cannot listen on socket" << std::endl; 
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool connect_socket() {
    int status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cerr << "Error: cannot connect to socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  int accept_connections() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
      return EXIT_FAILURE;
    }
    return client_connection_fd;
  }

  //recv message
  //Response r = parse_response() return this object
  //recv while loop to get entire response 

  // append_body() text in Response class - Adam
  // check_chunked_encoding()
  // content_length()

  //make_response() and forward to client socket 0.0.0.0 12345
  //close
  Response * receive_message(int client_connection_fd) {
    Response * response;
    do {
      char buffer[1024];
      ssize_t bytes = recv(client_connection_fd, buffer, 1024, 0);
      buffer[1024] = 0;

      std::vector<char> buffer_vector;
      buffer_vector.insert(buffer_vector.end(), buffer, buffer+1024);

      if (response == NULL) {
        response = parse_response(buffer_vector);
      } else {
        response->append_body(buffer_vector);
      }
      std::cout << buffer << std::endl; //PRINT FOR DEBUGGING
    } while (response->check_chunked_encoding());

    /*std::ofstream myfile;
    myfile.open ("log.txt");
    myfile << "Server received: " << buffer << std::endl;
    myfile.close();*/

    return response;
  }

  Response * client_receive() {
    return receive_message(socket_fd);
  }

  bool send_message(std::vector<char> message) {
    std::string buffer_string = std::string(message.begin(), message.end()); 
    const char * buffer = buffer_string.c_str();

    ssize_t status = send(socket_fd, buffer, message.size(), 0);    
    if (status == -1) {
      std::cerr << "Error: could not send message on socket" << std::endl;
      return false;
    }
    return true;
  }

  void close_socket() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  }
};