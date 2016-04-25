AR = ar
ARFLAGS = -r

# posix_memalign not declared in strict C99 so we have to use GNU99
CC      = mpiicc
CFLAGS  = -std=gnu99 -Wall -Wextra -Werror -I../include

# do not build malloc, just the mspaces API
CFLAGS += -DONLY_MSPACES

# use pthread_once to make initialization reentrant
CFLAGS += -DUSE_PTHREAD

# query MPI for nproc and threads
CFLAGS += -DUSE_MPI

FC = mpiifort

all: libhbwmalloc.a fortraninterface.o

libhbwmalloc.a: dlmalloc.o hbwmalloc.o
        $(AR) $(ARFLAGS) $@ dlmalloc.o hbwmalloc.o

dlmalloc.o: dlmalloc.c
        $(CC) $(CFLAGS) -c $< -o $@

hbwmalloc.o: hbwmalloc.c mspace.h
        $(CC) $(CFLAGS) -c $< -o $@

fortraninterface.o: fortraninterface.f90
        $(FC) -c $< -o $@
        rm $@

clean:
        -rm -f dlmalloc.o hbwmalloc.o fortran_hbwmalloc.mod
        -rm -f libhbwmalloc.a
