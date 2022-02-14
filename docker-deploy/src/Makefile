CPPFLAGS=-std=gnu++98 -pedantic -Wall -Werror -ggdb3

daemon: daemon.cpp daemon.hpp
	g++ $(CPPFLAGS) -o daemon daemon.cpp

clean:
	rm -f daemon
