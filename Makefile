CC 	= gcc
CFLAGS 	= -g
RM 	= rm -f

default: all

all: main.c
	$(CC) main.c -o matt_daemon
