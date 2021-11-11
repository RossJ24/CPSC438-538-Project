
CC = g++
FLAGS= -Wall -pedantic -I.
Server: server.cpp server.hpp
	${CC} server.cpp -o Server ${FLAGS}

clean:
	@rm -f Server