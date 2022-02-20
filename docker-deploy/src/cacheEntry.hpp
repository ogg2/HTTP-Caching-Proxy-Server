#ifndef __CACHE_ENTRY_HPP__
#define __CACHE_ENTRY_HPP__

#include <cstdlib>
#include <time.h>
#include <string>
#include "response.hpp"

/**
* Class to store cached responses and metadata of responses
*/
class CacheEntry {
private:
  time_t expiration; //tm_wday, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_year
  Response * response;

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
  //max-age is defined in Cache-Control Directives
  CacheEntry (int max_age) {
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

  Response * get_response() {
    if (is_fresh()) {
      return response;
    } else {
      return nullptr;
    }
  }

  //Updates expiration of cache response
  void update_expiration(int max_age) {
    time_t now;
    time (&now);
    expiration = now + max_age;
  }

  //update response information, potentially combine with update_expiration
  void update_response(Response * new_response) {
    response = new_response;
  }
  
  //max-stale is defined in Cache-Control Directives
  //if request directive indicates a max-stale value, client willing to accept
  //response that has exceeded freshness by max_stale seconds
  bool not_too_stale(int max_stale) {
    time_t now;
    time (&now);
    time_t allowable_stale = expiration + max_stale;
    return allowable_stale > now;
  }
  
  //min-fresh is defined in Cache-Control Directives
  //if request directive indicates a max-stale value, client requires
  //response will still be fresh for min_fresh more seconds
  bool will_be_fresh(int min_fresh) {
    time_t now;
    time (&now);
    now = now + min_fresh;
    return expiration > now;
  }
};

#endif
