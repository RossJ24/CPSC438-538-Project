
CC = g++
FLAGS= -g3
Server: controller_node.c server.o syscall_handlers.o
	${CC} ${FLAGS} -o Server server.o syscall_handlers.o controller_node.c 

server.o: server.hpp server.cpp 
	${CC} ${FLAGS} -c server.cpp

syscall_handlers.o: syscall_handlers.hpp syscall_handlers.cpp 
	${CC} ${FLAGS} -c syscall_handlers.cpp

clean:
	@rm -f Server
	@rm -f server.o
	@rm -f syscall_handlers.o