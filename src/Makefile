AR	= ar
ARFLAGS = -r

CC	= mpicc
CFLAGS	= -std=gnu99 -Wall -Wextra -Werror -I../include

# do not build malloc, just the mspaces API
CFLAGS += -DONLY_MSPACES

# use pthread_once to make initialization reentrant
CFLAGS += -DUSE_PTHREAD

# query MPI for nproc and threads
CFLAGS += -DUSE_MPI

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

