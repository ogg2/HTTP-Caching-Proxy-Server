#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

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
  std::vector<char> buffer = readFile(argv[1]);
  Request * r = parse_request(buffer);
  r->print();
  std::vector<char> new_req = r->make_request();
  std::cout << string(new_req.begin(), new_req.end()) << std::endl;
  
  return EXIT_SUCCESS;
}
