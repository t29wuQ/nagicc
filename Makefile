CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS: .c=.o)

nagicc: $(OBJS)
	$(CC) -o nagicc $(OBJS) $(LDFLAGS)

$(OBJS): nagicc.h

test: nagicc 
	./test.sh

clean:
	rm -f nagicc *.o *~ tmp*

.PHONY: test clean
