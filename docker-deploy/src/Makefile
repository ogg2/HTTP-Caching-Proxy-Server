CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

all: main-res main-req proxy test-cache test-response

main-res: main-res.cpp parse.cpp    
	g++ $(CPPFLAGS) -o main-res main-res.cpp parse.cpp    

main-req: main-req.cpp parse.cpp    
	g++ $(CPPFLAGS) -o main-req main-req.cpp parse.cpp    

proxy: proxy.cpp
	g++ $(CPPFLAGS) -o main proxy.cpp parse.cpp

#daemon: daemon.cpp daemon.hpp
#	g++ $(CPPFLAGS) -o daemon daemon.cpp

test-cache: test_cache.cpp
	g++ $(CPPFLAGS) -o test-cache test_cache.cpp

test-response: test_response.cpp parse.cpp
	g++ $(CPPFLAGS) -o test-response test_response.cpp parse.cpp

clean:
	rm -f daemon main main-req main-res test-cache test-response
	rm -f *~
	rm -r -f *.dSYM
