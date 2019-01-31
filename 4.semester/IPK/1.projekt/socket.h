/************************************
 * IPK 2018 - 1.Projekt 
 * Popis: Hlavickovy soubor pro klient-server komunikaci za ucelem ziskani informace o uzivatelich
 * File: socket.h
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 9.3.2018
 * Zdroje : Ryan Lattrel - Designing and Implementing an Application Layer Network Protocol Using UNIX Sockets and TCP
 *              - dostupne z: https://www.egr.msu.edu/classes/ece480/capstone/fall12/group02/documents/Ryan-Lattrel_App-Note.pdf
 *          Demo implementace - Sockets in C
 *              - dostupne z: https://wis.fit.vutbr.cz/FIT/st/course-files-st.php?file=%2Fcourse%2FIPK-IT%2Fother%2FDemo_C.zip&cid=11963
 * **********************************/

#ifndef SOCKET_H
#define SOCKET_H

#include "socket.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pwd.h>

#define BUFSIZE 256

// enum pre typ spravy
enum msg_type{	DATA = 11, 
				DATA_END = 19, 
				SERVER_ERR = 20, 
				LIST = 30, 
				HOME_DIR = 40, 
				USER_INFO = 50, 
				CONFIRM_END = 70, 
				CONNECT_REQ = 81, 
				CONNECT_OK = 85, 
				CONNECT_FAIL = 89};

// struktura socketu
typedef struct{
    int soc_fd;
    struct sockaddr_in addr;
    struct hostent *hp;
}Socket_t;

// struktura spravy
typedef struct {
    enum msg_type flag;
    char data[BUFSIZE];
}Message_t;

// alokovania pamate pre socket
Socket_t * initSocket();

// uvolnenie pamate 
void DestroySocket(Socket_t * soc);

// Posiela spravu <msg> dlzky <msg_size> na socket <client_soc>
int SendTCPMessage(int client_soc, char *msg, int msg_size);

// Prijma spravu a uklada ju do <buf> velkosto <buf_size> zo socketu <client_soc>
int RecieveTCPMessage(int client_soc, char *buf, int buf_size);

// Zatvara socket
int CloseSocket(Socket_t * soc);

// Pomocna funkcia pre kontrolu uzivatelom zadaneho cisla (portu)
int CheckNum(const char *str);

#endif
