#  Makefile
#  Riešenie IJC-DU1,  23.4.2017
#  Autor: Matúš Liščinský, FIT
#  Login: xlisci02
#  GCC: gcc 5.4.0 (merlin)
#  make - preklad
#  make clean - vymazanie suborov do stavu ako pred prekladom

# compilers & flags
CC=gcc
CppC=g++
CFLAGS=-std=c99 -pedantic -Wall -Wextra
CppFLAGS=-std=c++11 -pedantic -Wall

#all
PROGRAMS=tail tail2 wordcount wordcount-dynamic

# premenna pre zostavenie kniznic
LIB_FILES=hash_function.o htab_init.o htab_move.o htab_size.o htab_bucket_count.o htab_lookup_add.o htab_find.o htab_foreach.o htab_remove.o htab_clear.o htab_free.o


all: $(PROGRAMS)

# bin. subory
tail: tail.o 
	$(CC) $(CFLAGS) tail.o -o tail

tail2: tail2.o
	$(CppC) $(CppFLAGS) tail2.cc -o tail2

wordcount: wordcount.o io.o libhtab.a
	$(CC) $(CFLAGS) -L. io.o wordcount.o libhtab.a -static -o wordcount -lhtab

wordcount-dynamic: wordcount.o io.o libhtab.so
	$(CC) $(CFLAGS) -L. wordcount.o io.o libhtab.so -o wordcount-dynamic -lhtab

# *.o subory 
tail.o: tail.c
	$(CC) $(CFLAGS) -c tail.c -o tail.o

tail2.o: tail2.cc
	$(CppC) $(CppFLAGS) -c tail2.cc -o tail2.o

hash_function.o: hash_function.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c hash_function.c -o hash_function.o

htab_init.o: htab_init.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_init.c -o htab_init.o

htab_move.o: htab_move.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_move.c -o htab_move.o

htab_size.o: htab_size.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_size.c -o htab_size.o

htab_bucket_count.o: htab_bucket_count.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_bucket_count.c -o htab_bucket_count.o 

htab_lookup_add.o: htab_lookup_add.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_lookup_add.c -o htab_lookup_add.o

htab_find.o: htab_find.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_find.c -o htab_find.o

htab_foreach.o: htab_foreach.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_foreach.c -o htab_foreach.o

htab_remove.o: htab_remove.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_remove.c -o htab_remove.o

htab_clear.o: htab_clear.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_clear.c -o htab_clear.o

htab_free.o: htab_free.c private_hash_table.h hash_table.h
	$(CC) $(CFLAGS) -fPIC -c htab_free.c -o htab_free.o



# *.o soubory pre wordcount
io.o: io.c io.h
	$(CC) $(CFLAGS) -c io.c -o io.o

wordcount.o: wordcount.c io.h hash_table.h
	$(CC) $(CFLAGS) -c wordcount.c -o wordcount.o


# zostavenie kniznic
libhtab.a: $(LIB_FILES)
	ar rcs libhtab.a $(LIB_FILES)

libhtab.so: $(LIB_FILES)
	gcc $(CFLAGS) -shared -fPIC $(LIB_FILES) -o libhtab.so

# cistenie do stavu ako pred zostavenim 
clean:
	rm -r *.o $(PROGRAMS) libhtab.a libhtab.so
