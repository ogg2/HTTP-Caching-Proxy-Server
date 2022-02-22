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
  string port;
  string resource;
  string version;
  map<string, string> headers;
  vector<char> body;
  
public:
  Request(REQ_TYPES req_type_,
	  string hostname_,
	  string port_,
	  string resource_,
	  string version_,
	  map<string, string> headers_,
	  vector<char> body_)
    : req_type(req_type_),
      hostname(hostname_),
      port(port_),
      resource(resource_),
      version(version_),
      headers(headers_),
      body(body_.begin(), body_.end()) {
  }

  void print() {
    cout << req_type << " " << hostname;
    if (port.length() > 0) { cout << ":" << port; }
    cout << resource << " " << version << endl;
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
      cout << it->first << ": " << it->second << endl;
    }
    cout << endl << string(body.begin(), body.end()) << endl;
  }

  vector<char> make_request() {
    string req_ty;
    switch (req_type) {
    case GET:
      req_ty = "GET";
      break;
    case POST:
      req_ty = "POST";
      break;
    case CONNECT:
      req_ty = "CONNECT";
      break;
    default:
      req_ty = "BREAK";
      break;
    }
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
  

  ssize_t content_length() {
    map<string, string>::iterator it = headers.find("Content-Length");
    if (it == headers.end()) { return -1; }
    return stoi(it->second);
  }

  ssize_t body_length() {
    return body.size();
  }

  void append_body(vector<char> text) {
    copy(text.begin(), text.end(), back_inserter(body));
  }

  const char * get_hostname() {
    return hostname.c_str();
  }

  string get_resource() {
    return resource;
  }

  const char * get_port() {
    if (port.length() == 0) { return "80"; }
    return port.c_str();
  }

  REQ_TYPES get_type() {
    return req_type;
  }
};

#endif
