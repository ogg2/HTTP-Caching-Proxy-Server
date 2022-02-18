#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

#include "request.hpp"

size_t find_nth_char(string s, char c, int n);

vector<string> split_url(string url);

REQ_TYPES enum_req_type(string req_type);

Request * parse_request(const vector<char> req);

#endif
