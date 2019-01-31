# IPK 1 .Projekt

## Popis:
Klient-server aplikácia pre získanie informácií o užívateľoch.

## Preklad
Preklad vyžaduje nainštalovaný prekladač [GCC](https://gcc.gnu.org/install/).
Pre zostavenie použi príkaz **make** alebo **make all**.

	
	$ make | make all
		gcc -Wall -Wextra -Werror -pedantic ipk-server.c socket.c -o ipk-server
		gcc -Wall -Wextra -Werror -pedantic ipk-client.c socket.c -o ipk-client

## Spustenie
Server:

	$ ./ipk-server -p port 
Klient: 

	$ ./ipk-client -h hostname -p port -[n|f|l] login

 - ***port*** je obmedzený na rozpätie 0 - 65535 
 - ***hostname*** iba IPv4 alebo fully qualified domain name

## Autor

 - Matúš Liščinský (xlisci02@stud.fit.vutbr.cz)