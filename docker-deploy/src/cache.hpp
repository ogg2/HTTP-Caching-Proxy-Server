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
  time_t expiration; //tm_wday, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_year
  std::string response;

  //okay to just have HashMap where key is url string, value is cache object
  //lru cache, double linked list and hashmap
  //node content to be response 
  //linked list only if lru 
  //out of memory then reboot proxy

  bool is_fresh() {
    time_t now;
    time (&now);
    double time_to_expiration = difftime(expiration, now);
    return time_to_expiration > 0;
  }
public:
  Cache (int max_age) {
    time_t now;
    time (&now); 
    expiration = now + max_age;
  }

  void print_expiration() {
    std::cout << asctime(gmtime(&expiration));

    if (is_fresh()) {
      std::cout << "Response is still valid" << std::endl;
    } else {
      std::cout << "Response is expired" << std::endl;
    }
  }

  std::string get_response() {
    if (is_fresh()) {
      return response;
    } else {
      return NULL;
    }
  }

  void update_expiration(int max_age) {
    time_t now;
    time (&now);
    expiration = now + max_age;
  }

  void update_response(std::string new_response) {
    response = new_response;
  }
};

#endif
