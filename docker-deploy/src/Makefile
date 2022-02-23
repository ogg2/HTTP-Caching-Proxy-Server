CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

proxy: proxy.cpp process.cpp parse.cpp log.cpp
	g++ $(CPPFLAGS) -o proxy proxy.cpp process.cpp parse.cpp log.cpp

all: clean proxy

clean:
	rm -f daemon proxy main-req main-res test-cache test-response
	rm -f *~
	rm -r -f *.dSYM
