#ifndef PARSE_REQUESTS_H
#define PARSE_REQUESTS_H

#include "requests.hpp"

size_t find_nth_char(string s, char c, int n);

vector<string> split_url(string url);

REQ_TYPES enum_req_type(string req_type);

Request * parse_request(const vector<char> req);

#endif
