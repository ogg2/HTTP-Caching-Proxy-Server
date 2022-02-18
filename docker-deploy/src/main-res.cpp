#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "responses.hpp"
#include "parse_responses.hpp"

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
  Response * r = parse_response(buffer);
  r->print();
  std::vector<char> new_res = r->make_response();
  std::cout << string(new_res.begin(), new_res.end()) << std::endl;  
  return EXIT_SUCCESS;
}
