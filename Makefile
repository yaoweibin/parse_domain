

CFLAGS = -pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g

all : test_domain

test_domain : test_domain.o parse_domain.o
	gcc -o test_domain test_domain.o parse_domain.o

test_domain.o : test_domain.c parse_domain.h 
	gcc -o test_domain.o -c ${CFLAGS} test_domain.c

parse_domain.o : parse_domain.c parse_domain.h 
	gcc -o parse_domain.o -c ${CFLAGS} parse_domain.c

clean: 
	rm -f test_domain
	rm -f *.o
