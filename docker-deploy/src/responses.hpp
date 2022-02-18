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
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;


class Response {
private:
  string version = "HTTP/1.1";
  int status_code;
  string reason_phrase;
  map<string, string> headers;
  string body;
  
public:
  Response(int status_code_,
	   string reason_phrase_,
	   map<string, string> headers_,
	   string body_)
    : status_code(status_code_),
      reason_phrase(reason_phrase_),
      headers(headers_),
      body(body_) {
  }

  void print() {
    cout << version << " " << to_string(status_code) << " " << reason_phrase << endl;
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
      cout << it->first << ": " << it->second << endl;
    }
    cout << endl <<  body << endl;
  }

  vector<char> make_response() {
    vector<char> buffer;

    // first line
    string status_code_str = to_string(status_code);
    copy(version.begin(), version.end(), back_inserter(buffer));
    buffer.push_back(' ');
    copy(status_code_str.begin(), status_code_str.end(), back_inserter(buffer));
    buffer.push_back(' ');
    copy(reason_phrase.begin(), reason_phrase.end(), back_inserter(buffer));
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
