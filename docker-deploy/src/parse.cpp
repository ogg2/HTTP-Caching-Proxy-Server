#include <stdlib.h>
#include <stdio.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <iostream>
#include <vector>
#include <map>

#include "request.hpp"
#include "response.hpp"
#include "parse.hpp"

using namespace std;

size_t find_nth_char(string s, char c, int n) {
  size_t loc = s.find(c, 0);
  for (int i = 1; i < n; i++) { loc = s.find(c, loc+1); }
  return loc;
}


vector<string> split_url(vector<char> url) {
  vector<char>::const_iterator h = url.begin();
  vector<char>::const_iterator t = h;
  vector<char>::const_iterator end = url.end();

  vector<char>::const_iterator host_start = h, host_end = end;
  vector<char>::const_iterator port_start = end, port_end = end;
  vector<char>::const_iterator path_start = end, path_end = end;

  while ((t != end) && (*t != ':')) { ++t; }
  if ((*(t + 1) == '/') && (*(t + 2) == '/')) {
    t += 3;
    h = t;
    host_start = h;
    while ((t != end) && (*t != ':') && (*t != '/')) { ++t; }
    host_end = t;
    if (*t == ':') {	
      ++t;
      h = t;
      while ((t != end) && (*t != '/')) { ++t; }
      port_start = h;
      if (*t == '/') {
	port_end = t;
	path_start = t;
      }
    }
    else if (t != end) { path_start = t; }
  }
  else if (t != end) {
    host_end = t;
    ++t;
    h = t;
    while ((t != end) && (*t != '/')) { ++t; }
    port_start = h;
    port_end = t;
    if (t != end) { path_start = t; }
  }

  vector<string> ret;
  ret.push_back(string(host_start, host_end));
  ret.push_back(string(path_start, path_end));
  ret.push_back(string(port_start, port_end));
  return ret;
}


REQ_TYPES enum_req_type(string req_type) {
  if (req_type.compare("GET") == 0) { return GET; }
  if (req_type.compare("POST") == 0) { return POST; }
  if (req_type.compare("CONNECT") == 0) { return CONNECT; }
  return ELSE;
}


int parse_chunk(vector<char>& buffer) {
  vector<char>::const_iterator h = buffer.begin();
  vector<char>::const_iterator t = h;
  vector<char>::const_iterator end = buffer.end();
  
  vector<char> temp;
  int hex_val = 0;
  
  while ((t != end) && (*t != ';') && (*t != '\r') && (*t != '\n')) { ++t; }
  vector<char> hex_vect(h, t);
  hex_vect.push_back('\0');
  char * hex_str = &hex_vect[0];
  
  try {
    hex_val = stoi(hex_str, 0, 16);
  }
  catch (invalid_argument & ia) {
    cerr << "Invalid Argument: " << hex_str << endl; //ia.what() << endl;
    return -1;
  }
  
  while ((t != end) && (*t != '\r') && (*t != '\n')) { ++t; }
  if ((t != end) && (*t == '\r')) { ++t; }
  if ((t != end) && (*t == '\n')) { ++t; }
  h = t;
  for (int i = 0; i < hex_val; i++) {
    if (t == end) { break; }
    ++t;
  }
  copy(h, t, back_inserter(temp));
  
  while ((t != end) && (*t != '\r') && (*t == '\n')) { ++t; }
  if (t != end) { ++t; }
  if ((t != end) && (*t == '\n')) { ++t; }
  h = t;

  swap(temp, buffer);
  return hex_val;
}


Request * parse_request(const vector<char> req) {
  vector<char>::const_iterator h = req.begin();
  vector<char>::const_iterator t = h;
  vector<char>::const_iterator end = req.end();

  while ((t != end) && (*t != ' ')) { ++t; }
  REQ_TYPES req_type = enum_req_type(string(h, t));

  ++t;
  h = t;

  while ((t != end) && (*t != ' ')) { ++t; }
  vector<char> url;
  copy(h, t, back_inserter(url));
  vector<string> temp = split_url(url);
  string hostname = temp[0];
  string resource = temp[1];
  string port = temp[2];
  string full_url(h, t);
  
  ++t;
  h = t;

  while ((t != end) && ((*t != '\r'))) {
    if (*t == '\n') {  break; }
    ++t;
  }
  string version(h, t);

  ++t;
  if (*t == '\n') { ++t; }
  h = t;

  map<string, string> headers;
  while ((h != end) && (*h != '\r')) {
    if (*h == '\n') { break; }
    while ((t != end) && (*t != '\r')) {
      if (*t == '\n') { break; }
      ++t;
    }
    if (string(h, t).find(':') == string::npos) { break; }
    vector<char>::const_iterator v = h;
    while ((v != t) && (*v != ':')) { ++v; }
    string header_key(h, v);
    ++v;
    while ((v != t) && ((*v == ' ') || (*v == '\t'))) { ++v; }
    string header_val(v, t);

    headers.insert({header_key, header_val});

    if (t == end) { break; }
    if (*t == '\r') {
      ++t;
      h = t;
    }
    if ((t != end) && (*t == '\n')) {
      ++t;
      h = t;
    }

    if ((h == end) || (t == end)) { break; }

    if ((*h == ' ') || (*h == '\t')) {
      while ((t != end) && (*t != '\r') && (*t != '\n')) { ++t; }
      while ((h != end) && (h != t) && ((*h == ' ') || (*h == '\t'))) { ++h; }
      string prev_val = headers.find(header_key)->second;
      headers.erase(header_key);
      headers.insert({header_key, prev_val + string(h, t)});
    }

    if ((t != end) && (*t == '\r')) {
      ++t;
      h = t;
    }
    if ((t != end) && (*t == '\n')) {
      ++t;
      h = t;
    }
  }

  if ((h != end) && (*h == '\r')) { ++h; }
  if ((h != end) && (*h == '\n')) { ++h; }
  vector<char> body(h, end);

  return new Request(req_type, full_url, hostname, port, resource, version, headers, body);
}


Response * parse_response(const vector<char> resp) {
  vector<char>::const_iterator h = resp.begin();
  vector<char>::const_iterator t = h;
  vector<char>::const_iterator end = resp.end();

  while ((t != end) && (*t != ' ')) { ++t; }
  string version(h, t);

  ++t;
  h = t;

  while ((t != end) && (*t != ' ')) { ++t; }

  int status_code = 0;
  try {
    status_code = stoi(string(h, t));
  }
  catch (invalid_argument & ia) {
    cerr << "Invalid Argument: " << ia.what() << endl;
    return nullptr;
  }

  ++t;
  h = t;

  while ((t != end) && ((*t != '\r'))) {
    if (*t == '\n') {  break; }
    ++t;
  }
  string reason_phrase(h, t);

  ++t;
  if (*t == '\n') { ++t; }
  h = t;

  map<string, string> headers;
  while ((h != end) && (*h != '\r')) {
    if (*h == '\n') { break; }
    while ((t != end) && (*t != '\r')) {
      if (*t == '\n') { break; }
      ++t;
    }
    if (string(h, t).find(':') == string::npos) { break; }
    vector<char>::const_iterator v = h;
    while ((v != t) && (*v != ':')) { ++v; }
    string header_key(h, v);
    ++v;
    while ((v != t) && ((*v == ' ') || (*v == '\t'))) { ++v; }
    string header_val(v, t);

    headers.insert({header_key, header_val});

    if (t == end) { break; }
    if (*t == '\r') { ++t; }
    else if (*t == '\n') {
      ++t;
      h = t;
    }

    if ((*h == ' ') || (*h == '\t')) {
      while ((t != end) && (*t != '\r') && (*t != '\n')) { ++t; }
      while ((h != t) && ((*h == ' ') || (*h == '\t'))) { ++h; }
      string prev_val = headers.find(header_key)->second;
      headers.erase(header_key);
      headers.insert({header_key, prev_val + string(h, t)});

      if ((t != end) && (*t == '\r')) {
	++t;
	h = t;
      }
      if ((t != end) && (*t == '\n')) {
	++t;
	h = t;
      } 
    }
  }

  if ((h != end) && (*h == '\r')) { ++h; }
  if ((h != end) && (*h == '\n')) { ++h; }
  vector<char> body(h, end);

  return new Response(status_code, reason_phrase, headers, body);
}
