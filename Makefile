LIBCSP       = ./libcsp
LIBCSP_BUILD = ./libcsp/build
BUILD        = ./build
SHELL        = /bin/sh
CC           = gcc
RM           = rm -rf
MKDIR        = mkdir -p

csp-demo: clean server client

server: server.c
	$(MKDIR) $(BUILD)
	$(CC) -I $(LIBCSP)/include -pthread -o $(BUILD)/server server.c -L $(LIBCSP_BUILD) -l csp

client: client.c
	$(MKDIR) $(BUILD)
	$(CC) -I $(LIBCSP)/include -pthread -o $(BUILD)/client client.c -L $(LIBCSP_BUILD) -l csp

.PHONY: clean

clean:
	$(RM) $(BUILD)
