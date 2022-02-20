#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include "cacheEntry.hpp"
#include <unordered_map>

class Cache {
private:
  std::unordered_map<std::string, CacheEntry *> cache;

public:
  CacheEntry * findResponse(std::string url) {
    std::unordered_map<std::string, CacheEntry *>::const_iterator it = cache.find(url);
    if (it == cache.end()) {
      return nullptr;
    }
    return it->second;
  }

  void addEntry(std::string url, CacheEntry * entry) {
    if (cache.count(url) == 0) {
      cache.emplace(url, entry); 
    } else {
      std::unordered_map<std::string, CacheEntry *>::iterator it = cache.find(url);
      it->second = entry;
    }
  }
};

#endif
