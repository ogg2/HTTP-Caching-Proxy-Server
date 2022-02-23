#include <fstream>
#include <string>
#include <ctime>
#include <iostream>
//#include <boost/thread/mutex.hpp>
#include <mutex>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "log.hpp"

using namespace std;

string LOG_PATH = "./log/proxy.log";
//string LOG_PATH = "/var/log/erss/proxy.log";

void clear_log() {
  ofstream outfile(LOG_PATH);
}

string get_ip(int fd) {
  struct sockaddr_in ip_addr;
  socklen_t ip_len = sizeof(ip_addr);
  if (getpeername(fd, (struct sockaddr *)&ip_addr, &ip_len) == -1) { return ""; }

  char * ip = new char[20];
  strcpy(ip, inet_ntoa(ip_addr.sin_addr));
  if (ip == NULL) { return "IP ERROR"; }
  return ip;
}

void log_request(int fd, string request, mutex& log_mu) {
  time_t cur_time = time(nullptr);
  string cur_time_str = asctime(localtime(&cur_time));
  string ip = get_ip(fd);
  
  log_mu.lock();
  ofstream outfile;
  outfile.open(LOG_PATH, ios_base::app);
  outfile << fd << ": \"" << request << "\" from " << ip;
  outfile << " @ " << cur_time_str;
  log_mu.unlock();
}

void log_phrase(int fd, string phrase, mutex& log_mu) {
  if (fd <= 0) {
    log_mu.lock();
    ofstream outfile;
    outfile.open(LOG_PATH, ios_base::app);
    outfile << "(no-id): " << phrase << endl;
    log_mu.unlock();
    return;
  }
  
  log_mu.lock();
  ofstream outfile;
  outfile.open(LOG_PATH, ios_base::app);
  outfile << fd << ": " << phrase << endl;
  log_mu.unlock();
}

void log_origin_request(int fd, string request, string server, mutex& log_mu) {
  log_mu.lock();
  ofstream outfile;
  outfile.open(LOG_PATH, ios_base::app);
  outfile << fd << ": Requesting \"" << request << "\" from " << server << endl;
  log_mu.unlock();
}

void log_origin_response(int fd, string response, string server, mutex& log_mu) {
  log_mu.lock();
  ofstream outfile;
  outfile.open(LOG_PATH, ios_base::app);
  outfile << fd << ": Received \"" << response << "\" from " << server << endl;
  log_mu.unlock();
}

void log_response(int fd, string response, mutex& log_mu) {  
  log_mu.lock();
  ofstream outfile;
  outfile.open(LOG_PATH, ios_base::app);
  outfile << fd << ": Responding \"" << response << '\"' << endl;
  log_mu.unlock();
}

