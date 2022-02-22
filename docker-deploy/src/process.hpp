#ifndef PROCESS_H
#define PROCESS_H

#include <set>
#include <boost/thread/mutex.hpp>

#include "serverClient.hpp"

void process_request(ServerClient & server, int fd, std::set<int> & ids, Cache * cache, boost::mutex& log_mu);

#endif
