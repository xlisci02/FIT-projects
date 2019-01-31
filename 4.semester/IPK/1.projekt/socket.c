/************************************
 * IPK 2018 - 1.Projekt 
 * Popis: Hlavickovy soubor pro klient-server komunikaci za ucelem ziskani informace o uzivatelich
 * File: socket.c
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 9.3.2018
 * Zdroje : Ryan Lattrel - Designing and Implementing an Application Layer Network Protocol Using UNIX Sockets and TCP
 *              - dostupne z: https://www.egr.msu.edu/classes/ece480/capstone/fall12/group02/documents/Ryan-Lattrel_App-Note.pdf
 *          Demo implementace - Sockets in C
 *              - dostupne z: https://wis.fit.vutbr.cz/FIT/st/course-files-st.php?file=%2Fcourse%2FIPK-IT%2Fother%2FDemo_C.zip&cid=11963
 * **********************************/

#include "socket.h"
Socket_t * initSocket(){
	Socket_t *ptr;
	if(!(ptr = (Socket_t *) malloc(sizeof(Socket_t)))){
		fprintf(stderr,"ERR: malloc\n");
		exit(1);
	}
    return ptr;
}

void DestroySocket(Socket_t * soc){
    free(soc);
}

int SendTCPMessage(int client_soc, char *msg, int msg_size){
    int bytes = send(client_soc, msg, msg_size, 0);
    if (bytes < 0) 
      fprintf(stderr,"ERROR in sendto");
    return bytes;
}

int RecieveTCPMessage(int client_soc, char *buf, int buf_size){
    int bytes = recv(client_soc, buf, buf_size, 0);
  	// pocet prijatych bytov sa riesi az pri samotnom volani
    return bytes;
}

int CloseSocket(Socket_t * soc){
    close(soc->soc_fd);
    return 0;
}

int CheckNum(const char *str){
    char * pEnd;
    int value = (int) strtol(str, &pEnd,10);
    if(*pEnd != '\0')
        return -1;
    else return value; 
}