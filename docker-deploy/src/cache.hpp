#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <cstdlib>
#include <time.h>
#include <vector>

/**
* Class to store cached responses and metadata of reponses
*/
class Cache {
private:
  int id;
  bool validated;

  //call localtime() to populate
  //call asctime(localtime(t)) to print in required format
  struct tm * expiration_t; //tm_wday, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_year

  std::vector<char> response; //std::string response; instead ?

  bool is_fresh() {
    time_t now;
    time (&now);
    double time_to_expiration = difftime(mktime(&expiration), now);
    return time_to_expriation > 0;
  }
public:
  Cache (int id, int max_age) : id(id), validated(true) {
    time_t expiration;
    time (&expiration); 
    expiration += max_age;
    expiration_t = gmtime(&expiration);
  }
};

#endif
