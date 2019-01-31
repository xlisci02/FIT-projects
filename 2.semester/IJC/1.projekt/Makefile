#
#  Makefile
#  Riešenie IJC-DU1,  26.3.2017
#  Autor: Matúš Liščinský, FIT
#  Login: xlisci02
#  GCC: gcc 5.4.0 (merlin)
#  make - preklad
#  make run - spustenie stylom time ./primes .....
#  make clean - vymazanie suborov do stavu ako pred prekladom

CC= gcc
CFLAGS= -O2 -std=c99 -Wall -pedantic
INLINE= -DUSE_INLINE


#####################################################################################
# build all

all: primes primes-i steg-decode

steg-decode: ppm.o error.o steg-decode.o eratosthenes.o 
	$(CC) $(CFLAGS) steg-decode.o ppm.o error.o  eratosthenes.o -o steg-decode -lm

primes: primes.o error.o eratosthenes.o 
	$(CC) $(CFLAGS) primes.o error.o eratosthenes.o -o primes -lm

primes-i: primes-i.o error.o eratosthenes-i.o 
	$(CC) $(CFLAGS) $(INLINE) primes-i.o error.o eratosthenes-i.o -o primes-i -lm

#####################################################################################
# *.o subory

primes.o: primes.c eratosthenes.h bit_array.h
	$(CC) $(CFLAGS) -c primes.c -o primes.o

primes-i.o: primes.c eratosthenes.h bit_array.h
	$(CC) $(CFLAGS) $(INLINE) -c primes.c -o primes-i.o

eratosthenes.o: eratosthenes.c eratosthenes.h bit_array.h
	$(CC) $(CFLAGS) -c eratosthenes.c -o eratosthenes.o

eratosthenes-i.o: eratosthenes.c eratosthenes.h bit_array.h
	$(CC) $(CFLAGS) $(INLINE) -c eratosthenes.c -o eratosthenes-i.o

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o error.o

steg-decode.o: steg-decode.c ppm.h error.h bit_array.h eratosthenes.h
	$(CC) $(CFLAGS) -c steg-decode.c -o steg-decode.o

ppm.o: ppm.c ppm.h error.h
	$(CC) $(CFLAGS)  -c ppm.c -o ppm.o


#####################################################################################

run: 
	time ./primes
	time ./primes-i

clean: 
	rm -rf *.o 
	rm -rf primes primes-i steg-decode
