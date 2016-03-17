CC	= mpiicc
CFLAGS	= -std=c99 -O3 -xCORE-AVX2
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -I../include

LD	= $(CC)
LDFLAGS = $(CFLAGS)
LIBS	= -L../src -lhbwmalloc -lnuma

all: test_libnuma.x test_features.x test_performance.x

%.x: %.o ../src/libhbwmalloc.a
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c ../include/hbwmalloc.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f *.o
	-rm -f *.x

