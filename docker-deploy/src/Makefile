CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3

daemon: daemon.cpp daemon.hpp
	g++ $(CPPFLAGS) -o daemon daemon.cpp

clean:
	rm -f daemon
