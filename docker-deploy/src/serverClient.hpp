#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>

#include "request.hpp"
#include "response.hpp"
#include "parse.hpp"
#include "cache.hpp"

class ServerClient {
private:
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  const char * hostname;
  const char * port;
  Cache * cache;
  //get request - if in cache then return response

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
    status = ::bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
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

  void connect_tunnel(int client_connection_fd) {
    struct timeval timeout;
    timeout.tv_sec = 90; // make longer allow inactive for a bit?
    timeout.tv_usec = 0;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(client_connection_fd, &rfds);
    FD_SET(socket_fd, &rfds);
    int max_fd = client_connection_fd;

    ssize_t buffer_size = 1024;
    std::vector<char> buffer(buffer_size);
    int bytes;

    //while (status = select(2, &rfds, NULL, NULL, &timeout) { // loop to keep connection open until closed?
    while (true) {
      if (select(max_fd + 1, &rfds, NULL, NULL, &timeout) == -1) {
        std::cerr << "Error: could not CONNECT" << std::endl;
        return;
      }
      int new_fd;
      for (int i = 0; i <= max_fd; i++) {
        if (FD_ISSET(i, &rfds)) {
          if (i == client_connection_fd) {
            struct sockaddr_storage socket_addr;
            socklen_t socket_addr_len = sizeof(socket_addr);  
            new_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
            if (new_fd == -1) {
              std::cerr << "Error: cannot accept CONNECTion on socket" << std::endl;
              return;
            } else {
              FD_SET(new_fd, &rfds);
              if (new_fd > max_fd) {
                max_fd = new_fd;
              }
            }
          } else {
            if ((bytes = recv(i, &buffer.data()[0], buffer_size, 0)) <= 0) {
              if (bytes == 0) {
                return; //connection closed
              } else {
                std::cerr << "Error: cannot receive CONNECT data" << std::endl;
              }
            } else {
              for (int j = 0; j <= max_fd; j++) {
                if (FD_ISSET(j, &rfds)) { //&master??????
                  if (j == socket_fd) { //who are we sending this to?
                    std::string buffer_string = std::string(buffer.begin(), buffer.end());
                    const char * buffer_char = buffer_string.c_str();
                    if (send(j, buffer_char, bytes, 0) == -1) {
                      std::cerr << "Error: cannot send CONNECT data" << std::endl;
                    }
                  }
                }
              }
            }
          } //handle data from client
        } //new incoming connection
      } //int i - loop fds
    } //while loop
  }


  void recieve_chunked(Response * r, int fd) {
    ssize_t buffer_size = 1024;
    ssize_t bytes = 0;
    int length = 0;

    vector<char> buffer = r->get_body();
    int chunk_size = parse_chunk(buffer);
    if (chunk_size == -1) {
      std::cerr << "Invalid response from " << fd << std::endl;
      return;
    }
    length += chunk_size;
    if (chunk_size - buffer.size()) {
      std::vector<char> temp(buffer_size);
      bytes = recv(fd, &temp.data()[0], buffer_size, 0);
      if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; return; }
      if (bytes == 0) { return; }
      if (bytes < buffer_size) { buffer.resize(bytes); }
      buffer.resize(bytes);
      copy(temp.begin(), temp.end(), back_inserter(buffer));
    }
    if (chunk_size > 0) {
      r->update_body(buffer);
    }
    else {
      r->add_header(process_footers(buffer));
      r->remove_chunked();
      r->add_header("Content-Length", to_string(length));
      return;
    }

    do {
      buffer.resize(buffer_size);
      bytes = recv(fd, &buffer.data()[0], buffer_size, 0);
      
      if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; break; }
      if (bytes == 0) { break; }
      if (bytes < buffer_size) { buffer.resize(bytes); }
      
      chunk_size = parse_chunk(buffer);
      if (chunk_size == -1) {
	std::cerr << "Invalid response from " << fd << std::endl;
	continue;
      }
      if (chunk_size - buffer.size() > 0) {
        std::vector<char> temp(buffer_size);
	bytes = recv(fd, &temp.data()[0], buffer_size, 0);
	if (bytes == -1) { std::cerr << "Invalid response from " << fd << std::endl; break; }
	if (bytes == 0) { break; }
	if (bytes < buffer_size) { buffer.resize(bytes); }
	buffer.resize(bytes);
	copy(temp.begin(), temp.end(), back_inserter(buffer));
      }
      if (chunk_size == 0) {
	r->add_header(process_footers(buffer));
	r->remove_chunked();
	r->add_header("Content-Length", to_string(length));
	break;
      }
      else {
	r->append_body(buffer);
      }
    } while ((bytes > 0) || (chunk_size > 0));
  }
  

  Response * receive_response(int fd) {
    Response * response = nullptr;
    ssize_t buffer_size = 1024;
    std::vector<char> buffer(buffer_size);
    ssize_t bytes = 0;

    /*
    while ((num_of_bytes = recv( c_socket, memblock, file_buf, 0 )) > 0) {
        dest.write(memblock,num_of_bytes);
    }
    */
    
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

    /*std::ofstream myfile;
    myfile.open ("log.txt");
    myfile << "Server received: " << buffer << std::endl;
    myfile.close();*/

    return response;
  }


  Request * receive_request(int fd) {
    Request * request = nullptr;
    ssize_t buffer_size = 1024;
    std::vector<char> buffer(buffer_size);
    ssize_t bytes = 0;

    do {
      bytes = recv(fd, &buffer.data()[0], buffer_size, 0);

      //std::cout << fd << " recv\n";
      
      if (bytes == -1) { std::cerr << "error ahhh" << std::endl; break; }
      if (bytes == 0) { break; }
      if (bytes < buffer_size) { buffer.resize(bytes); }

      if (request == nullptr) {
        request = parse_request(buffer);
	//std::cout << fd << "parse\n";
      } else {
        request->append_body(buffer);
	//std::cout << fd << "append\n";
      }

      buffer.resize(buffer_size);

      if (request->content_length() == -1) { break; }

      //std::cout << fd << "before while check\n";

    } while ((request->body_length() < request->content_length()));

    /*std::ofstream myfile;
    myfile.open ("log.txt");
    myfile << "Server received: " << buffer << std::endl;
    myfile.close();*/
    //std::cout << fd << "outside loop\n";
    return request;
  }

  Response * client_receive() {
    return receive_response(socket_fd);
  }

  bool send_request(std::vector<char> message) {
    return send_response(message, socket_fd);
    /*std::string buffer_string = std::string(message.begin(), message.end()); 
    const char * buffer = buffer_string.c_str();

    ssize_t status = send(socket_fd, buffer, message.size(), 0);    
    if (status == -1) {
      std::cerr << "Error: could not send message on socket" << std::endl;
      return false;
    }
    return true;*/
  }

  bool send_response(std::vector<char> message, int fd) {
    std::string buffer_string = std::string(message.begin(), message.end()); 
    const char * buffer = buffer_string.c_str();

    ssize_t status = send(fd, buffer, message.size(), 0);    
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

  Cache * get_cache() {
    return cache;
  }
};

#endif
