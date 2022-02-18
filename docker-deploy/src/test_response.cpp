#include <iostream>
#include "serverClient.hpp"
#include <assert.h>
#include "parse_requests.hpp"
#include "requests.hpp"

std::vector<char> readFile(char * filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
    return buffer;
  }
  return buffer;
}

int main(int argc, char ** argv) {
  bool is_server = false;
  const char * hostname = "www.man7.org";
  const char * port = "80";
  ServerClient client(hostname, port);
  int status = client.initialize_socket(is_server);
  if (status == -1) {
    std::cout << "Failed to setup client" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<char> buffer = readFile(argv[1]);
  Request * r = parse_request(buffer);
  std::vector<char> new_req = r->make_get_req();  

  client.send_message(new_req);

  client.client_receive();

  return EXIT_SUCCESS;
}
