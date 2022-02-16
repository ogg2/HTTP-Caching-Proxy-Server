#include "daemon.hpp"
#include <thread>
#include <sstream>

#define LOGGING "Start Logging my task = %d\n"
#define THREADLOG "\tThread ID = %s\n"

void Daemon::first_fork() {
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
}

void Daemon::terminate_parent() {
  if (pid > 0) {
    printf("Parent pid = %d\n", getpid());
    exit(EXIT_SUCCESS);
  }
}

void Daemon::set_session_leader() {
  if (setsid() < 0) {
    exit(EXIT_FAILURE);
  }
}

void Daemon::catch_signals() {
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
}

void Daemon::second_fork() {
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
}

void Daemon::set_file_permissions() {
  umask(077);
}

void Daemon::change_to_root() {
  chdir("/");
}

void Daemon::close_file_descriptors() {
  for (x_fd = sysconf(_SC_OPEN_MAX); x_fd >= 0; x_fd--) {
    close (x_fd);
  }
}

void Daemon::logging() {
  openlog("erss/proxy", LOG_PID, LOG_DAEMON);
  //openlog("Logs", LOG_PID, LOG_USER);
}

void Daemon::server_message(Server server, int client_id) {
  server.receive_message(client_id);

  std::ostringstream oss;
  oss << std::this_thread::get_id();
  //syslog(LOG_INFO, THREADLOG, oss.str().c_str());
  std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
  sleep(10);
}

int main () { 
  Daemon d;

  /*d.first_fork();
  d.terminate_parent();
  d.set_session_leader(); //set child as session leader
  d.catch_signals(); //catch and ignore handle signals

  d.second_fork();
  d.terminate_parent();
  d.set_file_permissions();
  d.change_to_root(); //change workign directory to root
  d.close_file_descriptors(); 
  d.logging(); //log errors*/

  Server s;
  int ret = s.server_init();
  if (ret == EXIT_FAILURE) {
    std::cout << "Failed to initialize server." << std::endl;
    exit(EXIT_FAILURE);
  }
  int count = 0;
//  int thread_id = 0; 
  while (1) {
    sleep(2);
    int client_id = s.accept_connections();
    if (client_id == -1) {
      std::cerr << "Error: cannot accept connection on socket" << std::endl;
    } else {
      std::thread (d.server_message, s, client_id).detach();
      std::cout << "Start Logging my task = " << count++ << std::endl;
      //syslog(LOG_INFO, LOGGING, count++);
    }
    //process request
    //send response
  }
  //closelog();
  return EXIT_SUCCESS;
}
