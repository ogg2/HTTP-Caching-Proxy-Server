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
#include <unordered_map>
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

  void update_body(vector<char> text) {
    swap(text, body);
  }
  
  ssize_t content_length() {
    map<string, string>::iterator it = headers.find("Content-Length");
    if (it == headers.end()) { return -1; }
    return stoi(it->second);
  }

  bool is_chunked() {
    map<string, string>::iterator it = headers.find("Transfer-Encoding");
    if (it == headers.end()) { return false; }
    size_t pos = it->second.find("chunked");
    if (pos == string::npos) { return false; }
    string chunked = it->second.substr(pos, pos+7);
    if (chunked.compare("chunked") != 0) { return false; }
    return true;
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

  unordered_map<string, int> get_cache_control() {
    unordered_map<string, int> cache_directives;
    if (headers.count("Cache-Control") == 0) {
      return cache_directives;
    }
    map<string, string>::iterator it = headers.find("Cache-Control");
    string directives = it->second;
    
    size_t first = 0;
    size_t len = directives.length();
    size_t delim = directives.find_first_of("=,");
    while (delim != string::npos) {
      if ((delim < len) && (directives[delim] == '=')) {
        string key = directives.substr(first, delim - first);
	if (delim + 1 >= len) { break; }
        first = delim + 1;
        delim = directives.find_first_of(",", delim + 1);
	if (delim >= len) { break; }
        int seconds = atoi(directives.substr(first, delim).c_str());
        cache_directives.emplace(key, seconds);
      
      } else if ((delim < len) && (directives[delim] == ',')) {
        string key = directives.substr(first, delim - first);
        cache_directives.emplace (key, 0);
      }
      first = delim;
      if (delim + 2 < len) {
	delim = directives.find_first_of("=,", delim + 2);
      }   
    }
    return cache_directives;
  }

  int get_status() {
    return status_code;
  }

  string get_response_line() {
    return version + " " + to_string(status_code) + " " + reason_phrase;
  }

};

#endif
