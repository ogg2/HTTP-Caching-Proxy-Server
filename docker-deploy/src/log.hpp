#ifndef LOG_H
#define LOG_H

#include <string>
#include <mutex>
//#include <boost/thread/mutex.hpp>

using std::string; using std::mutex;

void clear_log();

string get_ip(int fd);

void log_request(int fd, string request, mutex& log_mu);

void log_phrase(int fd, string request, mutex& log_mu);

void log_origin_request(int fd, string request, string server, mutex& log_mu);

void log_origin_response(int fd, string response, string server, mutex& log_mu);

void log_response(int fd, string response, mutex& log_mu);

#endif
