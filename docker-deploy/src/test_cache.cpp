#include <iostream>
#include "cacheEntry.hpp"
#include <assert.h>

int main () {
  CacheEntry valid(100);
  valid.print_expiration();

  CacheEntry invalid(-40);
  invalid.print_expiration();

  std::cout << "Stale testing" << std::endl;
  CacheEntry stale(-10);
  assert (stale.not_too_stale(11));
  assert (!stale.not_too_stale(9));

  std::cout << "Min Fresh testing" << std::endl;
  CacheEntry min_fresh(20);
  assert (min_fresh.will_be_fresh(19));
  assert (!min_fresh.will_be_fresh(21));

  return EXIT_SUCCESS;
}
