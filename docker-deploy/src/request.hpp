#ifndef REQUEST_H
#define REQUEST_H

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
#include <vector>
#include <algorithm>
#include <iterator>

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
  vector<char> body;
  
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

  vector<char> make_get_req() {
    string req_ty = "GET";
    vector<char> buffer;

    // first line
    copy(req_ty.begin(), req_ty.end(), back_inserter(buffer));
    buffer.push_back(' ');
    copy(resource.begin(), resource.end(), back_inserter(buffer));
    buffer.push_back(' ');
    copy(version.begin(), version.end(), back_inserter(buffer));
    buffer.push_back('\r');
    buffer.push_back('\n');

    // headers
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
      string header_key = it->first;
      string header_val = it->second;
      copy(header_key.begin(), header_key.end(), back_inserter(buffer));
      buffer.push_back(':');
      buffer.push_back(' ');
      copy(header_val.begin(), header_val.end(), back_inserter(buffer));
      buffer.push_back('\r');
      buffer.push_back('\n');  
    }

    //body
    buffer.push_back('\r');
    buffer.push_back('\n');
    copy(body.begin(), body.end(), back_inserter(buffer));

    return buffer;
  }

};

#endif
