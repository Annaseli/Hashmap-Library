.PHONY: all, clean

CCFLAGS = -Wall -Wextra -Wvla -Werror -g -lm -std=c99

all: libhashmap.a libhashmap_tests.a

libhashmap.a: pair.o vector.o hashmap.o
	ar rcs libhashmap.a pair.o vector.o hashmap.o

libhashmap_tests.a: test_suite.o hashmap.o pair.o vector.o
	ar rcs libhashmap_tests.a test_suite.o hashmap.o pair.o vector.o

pair.o: pair.c pair.h
	gcc -c $(CCFLAGS) pair.c -o pair.o

vector.o: vector.c vector.h
	gcc -c $(CCFLAGS) vector.c -o vector.o

hashmap.o: hashmap.c hashmap.h
	gcc -c $(CCFLAGS) hashmap.c -o hashmap.o

test_suite.o: test_suite.c test_suite.h pair.h hash_funcs.h test_pairs.h
	gcc -c $(CCFLAGS) test_suite.c -o test_suite.o


clean:
	rm *.o *.a
