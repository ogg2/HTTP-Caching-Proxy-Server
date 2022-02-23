#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <unordered_map>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "cacheEntry.hpp"

typedef boost::shared_mutex shared_mu;
typedef boost::unique_lock< shared_mu >  write_lock;
typedef boost::shared_lock< shared_mu >  read_lock;

class Cache {
private:
  std::unordered_map<std::string, CacheEntry *> cache;
  boost::shared_mutex cache_mu;

public:
  CacheEntry * find_response(std::string resource) {
    read_lock r_lock(cache_mu);
    std::unordered_map<std::string, CacheEntry *>::const_iterator it = cache.find(resource);
    if (it == cache.end()) {
      return nullptr;
    }
    return it->second;
  }

  void add_entry(std::string resource, CacheEntry * entry) {
    write_lock w_lock(cache_mu);
    if (cache.count(resource) == 0) {
      cache.emplace(resource, entry); 
    } else {
      std::unordered_map<std::string, CacheEntry *>::iterator it = cache.find(resource);
      it->second = entry;
    }
  }

  void remove_entry(std::string resource) {
    cache.erase(resource);
  }
};

#endif
