#ifndef LOG_H
#define LOG_H

#include <string>
#include <boost/thread/mutex.hpp>

void clear_log();

string get_ip(int fd);

void log_request(int fd, string request, boost::mutex& log_mu);

void log_phrase(int fd, string request, boost::mutex& log_mu);

void log_origin_request(int fd, string request, string server, boost::mutex& log_mu);

void log_origin_response(int fd, string response, string server, boost::mutex& log_mu);

void log_response(int fd, string response, boost::mutex& log_mu);

#endif
