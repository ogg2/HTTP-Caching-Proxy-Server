#include <stdio.h>
#include <stdlib.h> //exit
#include <unistd.h> //fork
#include <string.h>
#include <sys/stat.h> //umask
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>

#include "server.hpp"

#define LOGGING "Start Logging my task = %d\n"

int main () { 
  pid_t pid;
  int x_fd;

  //1
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  //terminate parent
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  //2 - set child as session leader
  if (setsid() < 0) {
    exit(EXIT_FAILURE);
  }
 
  //3 - catch ignore handle signals
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  //4  
  pid = fork(); 
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  //terminate parent
  if (pid > 0) {
    printf("Parent pid = %d\n", getpid());
    exit(EXIT_SUCCESS);
  }

  //5 - file permissions
  umask(077);

  //6 - change working dir to root
  chdir("/");

  //7 - close all file descriptors 
  for (x_fd = sysconf(_SC_OPEN_MAX); x_fd >= 0; x_fd--) {
    close (x_fd);
  }

  //8 - log errors
  int count = 0; 
  openlog("erss/proxy", LOG_PID, LOG_DAEMON);
  //openlog("Logs", LOG_PID, LOG_USER);

  Server s;
  int ret = s.server_init();
  
  while (1) {
    sleep(2);
    s.accept_connections();
    syslog(LOG_INFO, LOGGING, count++);
  }
  closelog();
  return EXIT_SUCCESS;
}
