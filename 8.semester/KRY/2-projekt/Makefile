# projekt: RSA
# autor: Matus Liscinsky
# login: xlisci02
# mail: xlisci02@stud.fit.vutbr.cz
# datum: 2.5.2020

CC=g++
CFLAGS=-Wall -Wextra -std=c++11
LIBS=-lgmp -lgmpxx
PROG=kry
ZIPFILES=$(PROG).cpp Makefile doc.pdf test.py

all: $(PROG).cpp
	$(CC) $(CFLAGS) $(PROG).cpp -o $(PROG) $(LIBS)
clean:
	rm $(PROG)
test: all
	python3 test.py
pack: all clean
	zip xlisci02.zip $(ZIPFILES)
