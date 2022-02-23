#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <errno.h>

#include "request.hpp"
#include "response.hpp"
#include "parse.hpp"
#include "cache.hpp"
#include "log.hpp"

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
        return EXIT_SUCCESS;
      }
    }
    return EXIT_FAILURE;
  }

  bool get_address_info() {
    int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
      //std::cerr << "Error: cannot get address info for host" << std::endl;
      //std::cerr << "  (" << hostname << ", " << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool create_socket() {
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
      //std::cerr << "Error: cannot create socket" << std::endl;
      //std::cerr << "  (" << hostname << ", " << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool bind_socket() {
    int yes = 1;
    int status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = ::bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      //std::cerr << "Error: cannot bind socket" << std::endl;
      //std::cerr << "  (" << hostname << ", " << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool listen_socket() {
    int status = listen(socket_fd, 100);
    if (status == -1) {
      //std::cerr << "Error: cannot listen on socket" << std::endl; 
      //std::cerr << "  (" << hostname << ", " << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  bool connect_socket() {
    int status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      //std::cerr << "Error " << errno << ": cannot connect to socket" << std::endl;
      //std::cerr << "  (" << hostname << ", " << port << ")" << std::endl;
      return false;
    }
    return true;
  }

  int accept_connections() {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
      //std::cerr << "Error: cannot accept connection on socket" << std::endl;
      return EXIT_FAILURE;
    }
    return client_connection_fd;
  }

  void connect_tunnel(int client_connection_fd) {
    std::string connect_init = "HTTP/1.1 200 OK\r\n\r\n";
    const char * connect_init_char = connect_init.c_str();
    if (send(client_connection_fd, connect_init_char, std::strlen(connect_init_char), 0) == -1) {
      //std::cerr << "Error: cannot send initial CONNECT data" << std::endl;
      return;
    }

    while (true) {
      fd_set read_fds;
      fd_set write_fds;

      FD_ZERO(&read_fds);
      FD_ZERO(&write_fds);
      FD_SET(client_connection_fd, &read_fds);
      FD_SET(socket_fd, &read_fds);
      FD_SET(client_connection_fd, &write_fds);
      FD_SET(socket_fd, &write_fds);

      int max_fd = client_connection_fd;
      if (socket_fd > client_connection_fd) {
        max_fd = socket_fd;
      }

      if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) == -1) {
        //std::cerr << "Error: could not CONNECT" << std::endl;
        return;
      }

      ssize_t buffer_size = 10240;
      std::vector<char> buffer(buffer_size);
      int bytes;

      //client is ready so receive from client and send to origin
      if (FD_ISSET(socket_fd, &read_fds)) {
        if ((bytes = recv(socket_fd, &buffer.data()[0], buffer_size, 0)) <= 0) {
          if (bytes == -1) {
            //std::cerr << "Error: cannot receive CONNECT data" << std::endl;
          }
          return; //return if error or if recv returns 0 bytes (close)
        } 
        if (FD_ISSET(client_connection_fd, &write_fds)) {
          ssize_t status = send(client_connection_fd, &buffer.data()[0], bytes, 0);
          if (status == -1) {
            //std::cerr << "Error: cannot send CONNECT data" << std::endl;
            return;
          }
        }
      //origin is ready so receive from origin and send to client
      } else if (FD_ISSET(client_connection_fd, &read_fds)) {
        if ((bytes = recv(client_connection_fd, &buffer.data()[0], buffer_size, 0)) <= 0) {
          if (bytes == -1) {
            //std::cerr << "Error: cannot receive CONNECT data" << std::endl;
          }
          return; //return if error or if recv returns 0 bytes (close)
        }
        if (FD_ISSET(socket_fd, &write_fds)) {
          ssize_t status = send(socket_fd, &buffer.data()[0], bytes, 0);
          if (status == -1) {
            //std::cerr << "Error: cannot send CONNECT data" << std::endl;
            return;
          }
        }
      }     
    } //while loop
  }

  void recieve_chunked(Response * r, int fd) {
    ssize_t buffer_size = 1024;
    ssize_t bytes = 0;

    vector<char> buffer = r->get_body();
    vector<char> buffer_temp(buffer);
    int chunk_size = parse_chunk(buffer_temp);
    if (chunk_size == -1) {
      //std::cerr << "Invalid response from " << fd << std::endl;
      return;
    }
    if (chunk_size - buffer_temp.size() > 0) {
      std::vector<char> temp(buffer_size);
      bytes = recv(fd, &temp.data()[0], buffer_size, 0);
      if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; return; }
      if (bytes == 0) { return; }
      if (bytes < buffer_size) { temp.resize(bytes); }
      copy(temp.begin(), temp.end(), back_inserter(buffer));
      r->update_body(buffer);
    }
    if (chunk_size == 0) { return; }
    do {
      buffer.resize(buffer_size);
      bytes = recv(fd, &buffer.data()[0], buffer_size, 0);
      
      if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; break; }
      if (bytes == 0) { break; }
      if (bytes < buffer_size) { buffer.resize(bytes); }

      buffer_temp = buffer;
      chunk_size = parse_chunk(buffer_temp);
      if (chunk_size == -1) {
	//std::cerr << "Invalid response from " << fd << std::endl;
	continue;
      }
      if (chunk_size - buffer_temp.size() > 0) {
	std::vector<char> temp(buffer_size);
	bytes = recv(fd, &temp.data()[0], buffer_size, 0);
	if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; return; }
	if (bytes == 0) { return; }
	if (bytes < buffer_size) { temp.resize(bytes); }
	copy(temp.begin(), temp.end(), back_inserter(buffer));
      }
      r->append_body(buffer);
      if (chunk_size == 0) { return; }
    } while ((bytes > 0) || (chunk_size > 0));
  }
  

  Response * receive_response(int fd) {
    Response * response = nullptr;
    ssize_t buffer_size = 1024;
    std::vector<char> buffer(buffer_size);
    ssize_t bytes = 0;
    
    do {
      buffer.resize(buffer_size);
      bytes = recv(fd, &buffer.data()[0], buffer_size, 0);
      
      if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; break; }
      if (bytes == 0) { break; }
      if (bytes < buffer_size) { buffer.resize(bytes); }

      if (response == nullptr) {
	response = parse_response(buffer);
	if (response->is_chunked()) {
	  recieve_chunked(response, fd);
	  break;
	}
      } else {
	response->append_body(buffer);
      }

    } while ((response->body_length() < response->content_length())
	     || (bytes > 0));
    return response;
  }


  Request * receive_request(int fd) {
    Request * request = nullptr;
    ssize_t buffer_size = 1024;
    std::vector<char> buffer(buffer_size);
    ssize_t bytes = 0;

    do {
      bytes = recv(fd, &buffer.data()[0], buffer_size, 0);

      if (bytes == -1) { std::cerr << "error ahhh" << std::endl; break; }
      if (bytes == 0) { break; }
      if (bytes < buffer_size) { buffer.resize(bytes); }

      if (request == nullptr) {
        request = parse_request(buffer);
      } else {
        request->append_body(buffer);
      }

      buffer.resize(buffer_size);

    } while ((request->content_length() != -1)
	     && (request->body_length() < request->content_length()));
    
    return request;
  }

  Response * client_receive() {
    return receive_response(socket_fd);
  }

  bool send_request(std::vector<char> buffer) {
    return send_response(buffer, socket_fd);
  }

  bool send_response(std::vector<char> buffer, int fd) {
    ssize_t status = send(fd, &buffer.data()[0], buffer.size(), 0);    
    if (status == -1) {
      //std::cerr << "Error: could not send message on socket" << std::endl;
      return false;
    }
    return true;
  }

  void close_socket() {
    freeaddrinfo(host_info_list);
    close(socket_fd);
  }

  int get_fd() {
    return socket_fd;
  }
};

#endif
