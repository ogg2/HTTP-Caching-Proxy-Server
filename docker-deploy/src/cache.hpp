#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include "cacheEntry.hpp"
#include <unordered_map>

class Cache {
private:
  std::unordered_map<std::string, CacheEntry *> cache;

public:
  CacheEntry * find_response(std::string resource) {
    std::unordered_map<std::string, CacheEntry *>::const_iterator it = cache.find(resource);
    if (it == cache.end()) {
      return nullptr;
    }
    return it->second;
  }

  void add_entry(std::string resource, CacheEntry * entry) {
    if (cache.count(resource) == 0) {
      cache.emplace(resource, entry); 
    } else {
      std::unordered_map<std::string, CacheEntry *>::iterator it = cache.find(resource);
      it->second = entry;
    }
  }
};

#endif
