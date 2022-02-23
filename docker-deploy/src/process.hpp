#ifndef PROCESS_H
#define PROCESS_H

#include <set>
#include <boost/thread/mutex.hpp>

#include "serverClient.hpp"

Response * make_502_response();

Response * make_400_response();

void process_request(ServerClient & server, int fd, Cache * cache, boost::mutex& log_mu);

bool cache_revalidate(unordered_map<string, int> & directives, int fd, boost::mutex& log_mu);

int cache_max_age(unordered_map<string, int> & directives, int fd);

bool cache_no_store(unordered_map<string, int> & directives, int fd, boost::mutex& log_mu);

#endif
