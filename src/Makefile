AR	= ar
ARFLAGS = -r

# posix_memalign not declared in strict C99 so we have to use GNU99
CC	= mpiicc
CFLAGS	= -std=gnu99 -Wall -Wextra -Werror -I../include
CFLAGS += -O3 -xCORE-AVX2

# do not build malloc, just the mspaces API
CFLAGS += -DONLY_MSPACES

# use pthread_once to make initialization reentrant
CFLAGS += -DUSE_PTHREAD

# query MPI for nproc and threads
CFLAGS += -DUSE_MPI

all: libhbwmalloc.a

libhbwmalloc.a: dlmalloc.o hbwmalloc.o
	$(AR) $(ARFLAGS) $@ dlmalloc.o hbwmalloc.o

dlmalloc.o: dlmalloc.c
	$(CC) $(CFLAGS) -c $< -o $@

hbwmalloc.o: hbwmalloc.c mspace.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f dlmalloc.o hbwmalloc.o
	-rm -f libhbwmalloc.a

