#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

class Server {
  private:
    int socket_fd;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len;
    int client_connection_fd;
    struct addrinfo *host_info_list;

  public:
    int server_init() {
      int status;
      struct addrinfo host_info;
      const char *hostname = NULL;
      const char *port     = "4444";

      memset(&host_info, 0, sizeof(host_info));

      host_info.ai_family   = AF_UNSPEC;
      host_info.ai_socktype = SOCK_STREAM;
      host_info.ai_flags    = AI_PASSIVE;

      status = getaddrinfo(hostname, port, &host_info, &host_info_list);
      if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
      }

      socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
      if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
      }

      int yes = 1;
      status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
      if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
      }

      status = listen(socket_fd, 100);
      if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl; 
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
      }

      std::cout << "Waiting for connection on port " << port << std::endl;
      socket_addr_len = sizeof(socket_addr);
      return EXIT_SUCCESS;
    }

    int accept_connections() {
      std::ofstream myfile;
      myfile.open ("log.txt");
    
      client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
      if (client_connection_fd == -1) {
        std::cerr << "Error: cannot accept connection on socket" << std::endl;
        return EXIT_FAILURE;
      }

      char buffer[512];
      recv(client_connection_fd, buffer, 9, 0);
      buffer[9] = 0;

      myfile << "Server received: " << buffer << std::endl;
      myfile.close();
      return EXIT_SUCCESS;
    }

    void close_socket() {
      freeaddrinfo(host_info_list);
      close(socket_fd);
    }
};
