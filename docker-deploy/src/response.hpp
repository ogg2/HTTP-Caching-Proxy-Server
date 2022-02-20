#ifndef RESPONSE_H
#define RESPONSE_H

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
  vector<char> body;
  
public:
  Response(int status_code_,
	   string reason_phrase_,
	   map<string, string> headers_,
	   vector<char> body_)
    : status_code(status_code_),
      reason_phrase(reason_phrase_),
      headers(headers_),
      body(body_.begin(), body_.end()) {
  }

  void print() {
    cout << version << " " << to_string(status_code) << " " << reason_phrase << endl;
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
      cout << it->first << ": " << it->second << endl;
    }
    cout << endl << string(body.begin(), body.end()) << endl;
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

  void append_body(vector<char> text) {
    copy(text.begin(), text.end(), back_inserter(body));
  }

  void replace_body(vector<char> text) {
    swap(text, body);
  }

  bool check_chunked_encoding() {
    map<string, string>::iterator it = headers.find("Transfer-Encoding");
    if (it == headers.end()) { return false; }
    if (it->second.compare("chunked") == 0) { return true; }
    return false;
  }
  
  ssize_t content_length() {
    map<string, string>::iterator it = headers.find("Content-Length");
    if (it == headers.end()) { return -1; }
    return stoi(it->second);
  }
  
  void add_header(string key, string val) {
    headers.insert({key, val});
  }

  void add_header(map<string, string> map) {
    headers.insert(map.begin(), map.end());
  }

  ssize_t body_length() {
    return body.size();
  }

  vector<char> get_body() {
    return body;
  }

};

#endif
