#include <iostream>
#include "cache.hpp"

int main () {
  Cache valid(1, 100);
  valid.print_expiration();

  Cache invalid(2, -40);
  invalid.print_expiration();

  return EXIT_SUCCESS;
}
