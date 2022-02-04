# This is a very basic makefile for compiling our CSP demo. 
# We should improve this makefile later

SHELL       := /bin/sh
RM          := rm -rf
MKDIR       := mkdir -p
LIBCSP      := ./libcsp/include

all: 
    clean build

build:
	$(MKDIR) build
	$(CC) server.c -o build/server -I$(LIBCSP)
	$(CC) client.c -o build/client -I$(LIBCSP)

clean:
	$(RM) ./build