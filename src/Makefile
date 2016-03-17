AR	= ar
ARFLAGS = -r

CC	= gcc-5
CFLAGS	= -std=c99 -Wall -Wextra -Werror -I../include

all: libhbwmalloc.a

libhbwmalloc.a: dlmalloc.o hbwmalloc.o
	$(AR) $(ARFLAGS) $@ $<

dlmalloc.o: dlmalloc.c
	$(CC) -std=c89 -ansi -c $< -o $@

hbwmalloc.o: hbwmalloc.c mspace.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f dlmalloc.o hbwmalloc.o
	-rm -f libhbwmalloc.a

