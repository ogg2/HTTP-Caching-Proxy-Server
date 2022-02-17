#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <cstdlib>
#include <time.h>
#include <vector>

/**
* Class to store cached responses and metadata of responses
*/
class Cache {
private:
  int id;
  time_t expiration; //tm_wday, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_year
  std::vector<char> response; //std::string response; instead ?

  bool is_fresh() {
    time_t now;
    time (&now);
    double time_to_expiration = difftime(expiration, now);
    return time_to_expiration > 0;
  }
public:
  Cache (int id, int max_age) : id(id), validated(true) {
    time (&expiration); 
    expiration = expiration + max_age;
  }

  void print_expiration() {
    std::cout << asctime(gmtime(&expiration));

    if (is_fresh()) {
      std::cout << "Response " << id << " is still valid" << std::endl;
    } else {
      std::cout << "Response " << id << " is expired" << std::endl;
    }
  }
};

#endif
