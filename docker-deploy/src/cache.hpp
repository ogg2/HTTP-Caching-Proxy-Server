#ifndef __CACHE_HPP__
#define __CACHE_HPP__

#include <unordered_map>
#include <mutex>

#include "cacheEntry.hpp"
#include "log.hpp"

class Cache {
private:
  std::unordered_map<std::string, CacheEntry *> cache;
  std::mutex cache_mu;
  size_t cache_size = 0;
  const size_t max_size = 1000;

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

  void add_entry(std::string resource, CacheEntry * entry, std::mutex & log_mu) {
    cache_mu.lock();
    if (cache.count(resource) == 0) {
      cache.emplace(resource, entry);
      cache_size++;
      if (cache_size > max_size) {
        unordered_map<std::string, CacheEntry *>::iterator it = cache.begin();
        size_t count = 0;
        while (count < (max_size / 10)) {
          it = cache.erase(it);
          count++;
        }
        log_phrase(0, "NOTE clearing cache", log_mu);
        cache_size -= (max_size / 10);
      }
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
