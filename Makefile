LIBCSP       = ./libcsp
LIBCSP_BUILD = ./libcsp/build
LIBSOCKETCAN = ./libsocketcan
BUILD        = ./build
SHELL        = /bin/sh
CC           = gcc
RM           = rm -rf
MKDIR        = mkdir -p

csp-demo: csp-demo.c
	$(MKDIR) $(BUILD)
	$(CC) -I $(LIBCSP)/include -I $(LIBCSP_BUILD)/include -pthread -o $(BUILD)/csp-demo csp-demo.c -L $(LIBCSP_BUILD) -l csp

.PHONY: clean

clean:
	$(RM) $(BUILD)
