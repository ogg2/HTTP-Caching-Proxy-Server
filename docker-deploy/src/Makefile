CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

main: main.cpp
	g++ $(CPPFLAGS) -o main main.cpp

#daemon: daemon.cpp daemon.hpp
#	g++ $(CPPFLAGS) -o daemon daemon.cpp

test: test_cache.cpp
	g++ $(CPPFLAGS) -o test test_cache.cpp

clean:
	rm -f daemon test main
