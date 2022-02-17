#include <iostream>
#include "cache.hpp"
#include <assert.h>

int main () {
  Cache valid(100);
  valid.print_expiration();

  Cache invalid(-40);
  invalid.print_expiration();

  std::cout << "Stale testing" << std::endl;
  Cache stale(-10);
  assert (stale.not_too_stale(11));
  assert (!stale.not_too_stale(9));

  std::cout << "Min Fresh testing" << std::endl;
  Cache min_fresh(20);
  assert (min_fresh.will_be_fresh(19));
  assert (!min_fresh.will_be_fresh(21));

  return EXIT_SUCCESS;
}
