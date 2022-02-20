#include <stdlib.h>
#include <stdio.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>

#include "request.hpp"
#include "response.hpp"

using namespace std;

size_t find_nth_char(string s, char c, int n) {
  size_t loc = s.find(c, 0);
  for (int i = 1; i < n; i++) {
    loc = s.find(c, loc+1);
  }
  return loc;
}


vector<string> split_url(string url) {
  size_t host_loc = find_nth_char(url, '/', 2);
  size_t path_loc = find_nth_char(url, '/', 3);
  string http = url.substr(0, host_loc);
  string hostname = url.substr(host_loc+1, path_loc-host_loc-1);
  string pathname = url.substr(path_loc);

  vector<string> ret;
  ret.push_back(hostname);
  ret.push_back(pathname);
  return ret;

}


REQ_TYPES enum_req_type(string req_type) {
  if (req_type.compare("GET") == 0) {
    return GET;
  }
  if (req_type.compare("POST") == 0) {
    return POST;
  }
  if (req_type.compare("CONNECT") == 0) {
    return CONNECT;
  }
  return ELSE;
}


map<string, string> process_footers(vector<char> buffer) {

  vector<char>::const_iterator h = buffer.begin();
  vector<char>::const_iterator t = h;
  vector<char>::const_iterator end = buffer.end();

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
      t += 2;
      h = t;
    }
    if (*t == '\n') {
      t += 1;
      h = t;
    }

    if ((*h == ' ') || (*h == '\t')) {
      while ((t != end) && (*t != '\r')) {
	if (*t == '\n') { break; }
	++t;
      }
      while ((h != t) && ((*h == ' ') || (*h == '\t'))) {
	++h;
      }
      string prev_val = headers.find(header_key)->second;
      headers.erase(header_key);
      headers.insert({header_key, prev_val + string(h, t)});
    }

    if (t == end) { break; }
    if (*t == '\r') {
      t += 2;
      h = t;
    }
    if (*t == '\n') {
      t += 1;
      h = t;
    }
    
  }

  return headers;
}


int format_chunk(Response * r, bool first_chunk, vector<char> buffer) {
    if (!r->check_chunked_encoding()) { return -1; }

    if (first_chunk) { buffer = r->get_body(); }

    vector<char>::const_iterator h = buffer.begin();
    vector<char>::const_iterator t = h;
    vector<char>::const_iterator end = buffer.end();

    vector<char> temp;
    int hex_val = 0;

    while ((t != end) && (*t != ';')) {
      if (*t == '\r') {  break; }
      if (*t == '\n') {  break; }
      ++t;
    }
    string hex_str(h, t);
    
    try {
      hex_val = stoi(hex_str, 0, 16);
    }
    catch (invalid_argument & ia) {
      cerr << "Invalid Argument: " << ia.what() << endl;
      return -1;
    }
	   
    while ((t != end) && (*t != '\n')) { ++t; }
    ++t;
    h = t;
    for (int i = 0; i < hex_val; i++) {
      if (t == end) { break; }
      ++t;
    }
    copy(h, t, back_inserter(temp));

    while ((t != end) && (*t != '\r')) {
      if (*t == '\n') { break; }
      ++t;
    }
    ++t;
    if (*t == '\n') { ++t; }
    h = t;

    if (first_chunk) {
      r->replace_body(temp);
    }
    else {
      r->append_body(temp);
    }

    if (hex_val == 0) {
      map<string, string> footers = process_footers(vector<char>(h, end));
      r->add_header(footers);
    }
    
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
  string url(h, t);
  vector<string> temp = split_url(url);
  string hostname = temp[0];
  string resource = temp[1];

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
      t += 2;
      h = t;
    }
    if (*t == '\n') {
      t += 1;
      h = t;
    }

    if ((*h == ' ') || (*h == '\t')) {
      while ((t != end) && (*t != '\r')) {
	if (*t == '\n') { break; }
	++t;
      }
      while ((h != t) && ((*h == ' ') || (*h == '\t'))) {
	++h;
      }
      string prev_val = headers.find(header_key)->second;
      headers.erase(header_key);
      headers.insert({header_key, prev_val + string(h, t)});
    }

    if (t == end) { break; }
    if (*t == '\r') {
      t += 2;
      h = t;
    }
    if (*t == '\n') {
      t += 1;
      h = t;
    }
  }

  if (*h == '\r') {
    ++h;
  }
  if (*h == '\n') {
    ++h;
  }
  vector<char> body(h, end);

  return new Request(req_type, hostname, resource, version, headers, body);
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
  int status_code = stoi(string(h, t));

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
    if (*t == '\r') {
      t += 2;
      h = t;
    }
    else if (*t == '\n') {
      t += 1;
      h = t;
    }

    if ((*h == ' ') || (*h == '\t')) {
      while ((t != end) && (*t != '\r')) {
	if (*t == '\n') { break; }
	++t;
      }
      while ((h != t) && ((*h == ' ') || (*h == '\t'))) {
	++h;
      }
      string prev_val = headers.find(header_key)->second;
      headers.erase(header_key);
      headers.insert({header_key, prev_val + string(h, t)});

      
      if (t == end) { break; }
      if (*t == '\r') {
	t += 2;
	h = t;
      }
      else if (*t == '\n') {
	t += 1;
	h = t;
      } 
    }
  }

  if (*h == '\r') {
    ++h;
  }
  if (*h == '\n') {
    ++h;
  }
  vector<char> body(h, end);

  return new Response(status_code, reason_phrase, headers, body);
}
