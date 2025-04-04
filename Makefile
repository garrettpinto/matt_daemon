CC 	= gcc
CFLAGS 	= -g
RM 	= rm -f

default: all

all: main.c
	$(CC) main.c -o Matt_daemon
