#ifndef LOG_H
#define LOG_H

#include <string>
#include <boost/thread/mutex.hpp>

void log_request(int fd, string request, boost::mutex& log_mu);

#endif
