#include <fstream>
#include <string>
#include <ctime>

using namespace std;

void write_log(int fd, string phrase) {
  time_t cur_time = time(nullptr);
  
  std::ofstream outfile;
  outfile.open("log.txt", std::ios_base::app);
  outfile << "[" << fd << "]: " << phrase;
  outfile << " at " << asctime(localtime(&cur_time));
}
