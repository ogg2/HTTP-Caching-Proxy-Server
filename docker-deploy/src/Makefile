CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread -I /opt/homebrew/Cellar/boost/1.78.0/include
BOOSTFLAGS=-lboost_system -lboost_thread-mt
LDFLAGS=-L/opt/homebrew/lib/

proxy: proxy.cpp process.cpp parse.cpp log.cpp
	g++ $(CPPFLAGS) $(BOOSTFLAGS) $(LDFLAGS) -o proxy proxy.cpp process.cpp parse.cpp log.cpp

all: clean proxy

clean:
	rm -f daemon proxy main-req main-res test-cache test-response
	rm -f *~
	rm -r -f *.dSYM
