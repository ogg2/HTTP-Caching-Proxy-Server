CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread
BOOSTFLAGS=-lboost_system -lboost_thread-mt
HDFLAG=-I/opt/homebrew/Cellar/boost/1.78.0/include
LDFLAG=-L/opt/homebrew/lib/

proxy: proxy.cpp process.cpp parse.cpp log.cpp
	g++ $(CPPFLAGS) -o proxy proxy.cpp process.cpp parse.cpp log.cpp

all: clean proxy

clean:
	rm -f daemon proxy main-req main-res test-cache test-response
	rm -f *~
	rm -r -f *.dSYM
