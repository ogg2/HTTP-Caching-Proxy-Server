#ifndef PROCESS_H
#define PROCESS_H

#include <set>

#include "serverClient.hpp"

void proccess_request(ServerClient & server, int fd, std::set<int> & ids);

#endif
