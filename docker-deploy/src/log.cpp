#include <fstream>
#include <string>
#include <ctime>
#include <boost/thread/mutex.hpp>
#include <sys/socket.h>

using namespace std;

void log_request(int fd, string request, boost::mutex& log_mu) {
  struct sockaddr ip_addr;
  socklen_t ip_len = 14;
  if (getpeername(fd, &ip_addr, &ip_len) == -1) { return; }

  string ip(ip_addr.sa_data, ip_addr.sa_data + ip_len);
  time_t cur_time = time(nullptr);

  log_mu.lock();
  std::ofstream outfile;
  //outfile.open("/var/log/erss/proxy.log", std::ios_base::app);
  outfile.open("./log/proxy.log", std::ios_base::app);
  outfile << fd << ": " << request << "from" << ip;
  outfile << " @ " << asctime(localtime(&cur_time));
  log_mu.unlock();
}
