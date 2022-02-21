#include <assert.h>

#include <iostream>

#include "serverClient.hpp"
#include "request.hpp"
#include "parse.hpp"

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
  if (argc != 3) {
    std::cout << "Usage: ./test_response <input file> <hostname>" << std::endl;
  }
  bool is_server = false;
  const char * hostname = argv[2];
  const char * port = "80";
  ServerClient client(hostname, port);
  int status = client.initialize_socket(is_server);
  if (status == -1) {
    std::cout << "Failed to setup client" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<char> buffer = readFile(argv[1]);
  Request * request = parse_request(buffer);
  std::vector<char> new_request = request->make_request();  
  std::cout << string(new_request.begin(), new_request.end()) << std::endl;

  client.send_request(new_request);

  Response * response = client.client_receive();
  response->print();

  return EXIT_SUCCESS;
}
