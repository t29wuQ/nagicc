CFLAGS=-std=c11 -g -static

nagicc: nagicc.c

test: nagicc 
	./test.sh

clean:
	rm -f nagicc *.o *~ tmp*

.PHONY: test clean
