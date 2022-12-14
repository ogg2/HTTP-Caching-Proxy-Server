#ifndef PARSE_H
#define PARSE_H

#include "request.hpp"
#include "response.hpp"

size_t find_nth_char(string s, char c, int n);

vector<string> split_url(vector<char> url);

REQ_TYPES enum_req_type(string req_type);

int parse_chunk(vector<char>& buffer);

Request * parse_request(const vector<char> req);

Response * parse_response(const vector<char> resp);

#endif
