src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

all: epoll client

server: epoll.o 
	gcc epoll.o -o epoll -Wall 
client: client.o wrap.o 
	gcc client.o wrap.o -o client -Wall -lpthread

%.o:%.c
	gcc -c $< -Wall

.PHONY: clean all
clean: 
	-rm -rf epoll client $(obj)

