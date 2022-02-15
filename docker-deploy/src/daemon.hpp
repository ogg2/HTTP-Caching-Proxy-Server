#ifndef __DAEMON_HPP__
#define __DAEMON_HPP__

#include <stdio.h>
#include <stdlib.h> //exit
#include <unistd.h> //fork
#include <string.h>
#include <sys/stat.h> //umask
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>

#include "server.hpp"

class Daemon {
private:
  pid_t pid;
  int x_fd;

public:
  void first_fork();
  void set_session_leader();
  void catch_signals();
  void second_fork();
  void terminate_parent();
  void set_file_permissions();
  void change_to_root();
  void close_file_descriptors();
  void logging();
  static void server_message(Server server, int client_id);
};

#endif
