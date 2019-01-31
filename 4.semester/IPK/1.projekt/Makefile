#  Makefile
#  IPK 2018 - 1.Projekt 
#  Autor: Matus Liscinsky
#  Login: xlisci02
#  Mail: xlisci02@stud.fit.vutbr.cz
#  Datum: 9.3.2018
#  make - preklad
#  make clean - zmaze vytvorene binarky

CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic
all: ipk-server ipk-client

ipk-server: socket.h ipk-server.c socket.c
	$(CC) $(CFLAGS) ipk-server.c socket.c -o ipk-server
ipk-client: socket.h ipk-client.c socket.c
	$(CC) $(CFLAGS) ipk-client.c socket.c -o ipk-client

clean: 
	@rm ipk-client ipk-server