CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

daemon: daemon.cpp daemon.hpp
	g++ $(CPPFLAGS) -o daemon daemon.cpp

clean:
	rm -f daemon
