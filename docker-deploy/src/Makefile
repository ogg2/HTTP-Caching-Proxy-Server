CPPFLAGS=-std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

main-req: main-req.cpp
	g++ $(CPPFLAGS) -o main-req main-req.cpp parse_requests.cpp    

main: main.cpp
	g++ $(CPPFLAGS) -o main main.cpp

#daemon: daemon.cpp daemon.hpp
#	g++ $(CPPFLAGS) -o daemon daemon.cpp

clean:
	rm -f daemon
