CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

all: main-res main-req main

main-res: main-res.cpp parse_responses.cpp    
	g++ $(CPPFLAGS) -o main-res main-res.cpp parse_responses.cpp    

main-req: main-req.cpp parse_requests.cpp    
	g++ $(CPPFLAGS) -o main-req main-req.cpp parse_requests.cpp    

main: main.cpp
	g++ $(CPPFLAGS) -o main main.cpp

#daemon: daemon.cpp daemon.hpp
#	g++ $(CPPFLAGS) -o daemon daemon.cpp

test: test_cache.cpp
	g++ $(CPPFLAGS) -o test test_cache.cpp

test-response: test_response.cpp parse_requests.cpp
	g++ $(CPPFLAGS) -o test-response test_response.cpp parse_requests.cpp

clean:
	rm -f daemon test main main-req
