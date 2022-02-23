#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <unordered_map>
#include <mutex>

#include "cacheEntry.hpp"

class Cache {
private:
  std::unordered_map<std::string, CacheEntry *> cache;
  std::mutex cache_mu;

public:
  CacheEntry * find_response(std::string resource) {
    cache_mu.lock();
    std::unordered_map<std::string, CacheEntry *>::const_iterator it = cache.find(resource);
    if (it == cache.end()) {
      cache_mu.unlock();
      return nullptr;
    }
    cache_mu.unlock();
    return it->second;
  }

  void add_entry(std::string resource, CacheEntry * entry) {
    cache_mu.lock();
    if (cache.count(resource) == 0) {
      cache.emplace(resource, entry);
      cache_mu.unlock();
    } else {
      std::unordered_map<std::string, CacheEntry *>::iterator it = cache.find(resource);
      it->second = entry;
      cache_mu.unlock();
    }
  }

  void remove_entry(std::string resource) {
    cache_mu.lock();
    cache.erase(resource);
    cache_mu.unlock();
  }
};

#endif
