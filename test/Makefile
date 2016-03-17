CC	= gcc
CFLAGS	= -std=c99 -Wall -Wextra -Werror -g

LD	= $(CC)
LDFLAGS = $(CFLAGS)
LIBS	= -lnuma

all: test_libnuma.x

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CLFAGS) -c $< -o $@

clean:
	-rm -f *.o
	-rm -f *.x

