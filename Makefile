#------------------------------------------------------------------------------
#  Makefile for httpserver
#
#      make             makes httpserver
#      make clean       removes object files
#
#------------------------------------------------------------------------------

CC = gcc
CFLAGS= -Wall -Wextra -Wpedantic -Wshadow -pthread -lm

all: server

server:
	$(CC) loadbalancer.c Queue.c thread.c threadHelper.c List.c -o loadbalancer $(CFLAGS)

clean:
	rm httpserver
