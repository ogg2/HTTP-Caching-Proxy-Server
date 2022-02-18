CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

all: main-res main-req main

main-res: main-res.cpp parse_response.cpp    
	g++ $(CPPFLAGS) -o main-res main-res.cpp parse_response.cpp    

main-req: main-req.cpp parse_request.cpp    
	g++ $(CPPFLAGS) -o main-req main-req.cpp parse_request.cpp    

main: main.cpp
	g++ $(CPPFLAGS) -o main main.cpp

#daemon: daemon.cpp daemon.hpp
#	g++ $(CPPFLAGS) -o daemon daemon.cpp

test: test_cache.cpp
	g++ $(CPPFLAGS) -o test test_cache.cpp

clean:
	rm -f daemon test main
