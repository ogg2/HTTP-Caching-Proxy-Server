#include <iostream>
#include "cache.hpp"

int main () {
  Cache valid(100);
  valid.print_expiration();

  Cache invalid(-40);
  invalid.print_expiration();

  return EXIT_SUCCESS;
}
