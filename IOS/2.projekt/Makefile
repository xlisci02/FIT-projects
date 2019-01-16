#  Makefile
#  Riešenie IOS-Projekt2,  28.4.2017
#  Autor: Matúš Liščinský, FIT
#  Login: xlisci02
#  make - preklad
#  make clean - vymazanie suborov do stavu ako pred prekladom


# prekladac
CC=gcc

# flags								
CFLAGS=-std=gnu99 -Wall -Werror -Wextra -pedantic

all: proj2

proj2: proj2.o proj2.h
	$(CC) $(CFLAGS) proj2.o -o proj2 -lpthread -lrt

proj2.o: proj2.c
	$(CC) $(CFLAGS) -c proj2.c -o proj2.o

clean:
	rm -f *.o proj2
