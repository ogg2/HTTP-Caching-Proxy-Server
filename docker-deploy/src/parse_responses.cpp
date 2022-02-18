#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "responses.hpp"
#include "requests.hpp"

using namespace std;


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
  string body(h, end);

  return new Response(status_code, reason_phrase, headers, body);
}
