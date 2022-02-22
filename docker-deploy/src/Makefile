CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread -I /opt/homebrew/Cellar/boost/1.78.0/include

proxy: proxy.cpp process.cpp parse.cpp log.cpp
	g++ $(CPPFLAGS) -o proxy proxy.cpp process.cpp parse.cpp log.cpp

main-res: main-res.cpp parse.cpp    
	g++ $(CPPFLAGS) -o main-res main-res.cpp parse.cpp    

main-req: main-req.cpp parse.cpp    
	g++ $(CPPFLAGS) -o main-req main-req.cpp parse.cpp    

test-cache: test_cache.cpp
	g++ $(CPPFLAGS) -o test-cache test_cache.cpp

test-response: test_response.cpp parse.cpp
	g++ $(CPPFLAGS) -o test-response test_response.cpp parse.cpp


all: main-res main-req proxy test-cache test-response

clean:
	rm -f daemon proxy main-req main-res test-cache test-response
	rm -f *~
	rm -r -f *.dSYM



#daemon: daemon.cpp daemon.hpp
#	g++ $(CPPFLAGS) -o daemon daemon.cpp
