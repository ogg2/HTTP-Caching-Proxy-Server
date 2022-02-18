#ifndef REQUESTS_H
#define REQUESTS_H

#include <stdio.h>
#include <stdlib.h> //exit
#include <unistd.h> //fork
#include <sys/stat.h> //umask
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>

#include <string>
#include <iostream>
#include <map>

using namespace std;


enum REQ_TYPES : int {
  GET,
  POST,
  CONNECT,
  ELSE
};

class Request {
private:
  REQ_TYPES req_type;
  string hostname;
  string resource;
  string version;
  map<string, string> headers;
  string body;

  //char * make_get_req() {}
  
public:
  Request(REQ_TYPES req_type_,
	  string hostname_,
	  string resource_,
	  string version_,
	  map<string, string> headers_,
	  string body_)
    : req_type(req_type_),
      hostname(hostname_),
      resource(resource_),
      version(version_),
      headers(headers_),
      body(body_) {
  }

  void print() {
    cout << req_type << " " << hostname << resource << " " << version << endl;
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
      cout << it->first << ": " << it->second << endl;
    }
    cout << endl <<  body << endl;
  }

};

#endif
