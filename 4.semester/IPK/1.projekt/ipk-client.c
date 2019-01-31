/************************************
 * IPK 2018 - 1.Projekt 
 * Popis: Klient pro ziskani informace o uzivatelich
 * File: ipk-client.c
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
/*
 * Kontroluje spravnost parametrov, pri neuspechu vracia 1, inak 0
 */
int checkArguments(int argc, const char * argv [], const char ** host, const char ** port , const char ** login, Message_t * msg, int *port_number){
    int c;
    int h=0, p=0, n=0, f=0, l=0;
    int err=0; // sluzi ako indikator chyby
    const char * usage_msg = "Usage: ./ipk-client -h <host> -p <port> [-n|-f|-l] <login> \n";
    while ((c = getopt (argc,(char * const*)argv, "h:p:n:f:l")) != -1){
    switch (c){
      case 'h':
        if(h)
            err = 1; 
        else h = 1;
        *host = optarg;
        break;
      case 'p':
        if(p)
            err = 1; 
        else p = 1;
        *port = optarg;
        break;
      case 'n':
        if(n)
            err = 1; 
        else n = 1;
        *login = optarg;
        msg->flag = USER_INFO;
        break;
      case 'f':
        if(f)
            err = 1; 
        else f = 1;
        *login = optarg;
        msg->flag = HOME_DIR;
        break;
      case 'l':
        if(l)
            err = 1; 
        else l = 1;
        if(argc == 7)
            *login = argv[optind];
        msg->flag = LIST;
        break;
      default:
        fprintf(stderr, "%s", usage_msg );
        return 1; 
      }
    }
    if(err || (n && f) || (n && l) || (l && f)){
        fprintf(stderr, "%s", usage_msg );
        return 1;
    }
    if(argc!=7 && !(l && argc==6 && *login == NULL )){
        fprintf(stderr, "%s", usage_msg );
        return 1;
    }
    
    *port_number = CheckNum(*port);
    if(*port_number < 0 || *port_number > 65535){
        fprintf(stderr, "ERR: port out of range\n");
        return 1;
    }
    return 0;
    
}
/*
 * Funkcia pre pripojenie na server 
 */
Socket_t * ConnectToTCPServer(int port, const char * server_hostname){
    
    // Vytvaranie soketu
    Socket_t * soc = initSocket();
    
    if ((soc->soc_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("ERROR: soc");
        exit(1);
    }

    // ziskanie adresy
    if ((soc->hp = gethostbyname(server_hostname)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", server_hostname);
        exit(1);
    }
    soc->addr.sin_family = AF_INET; // nastavenie socket family
    memcpy(&(soc->addr.sin_addr), soc->hp->h_addr, soc->hp->h_length); // nastavenie adresy 
    soc->addr.sin_port = htons(port); // htons konvertuje na network byte order

    // connect 
    if (connect(soc->soc_fd, (struct sockaddr *) &(soc->addr), sizeof(soc->addr)) != 0)
    {
        perror("ERROR: connect");
        close(soc->soc_fd);
        exit(1);        
    }
    return soc;
}

int main (int argc, const char * argv[]) {
    Message_t  msg = { DATA,{'\0'}}; // sprava pre posielanie dat
    Message_t  conn_msg = { CONNECT_REQ,{'\0'}}; // sprava pre overenie pripojenia
    
    int port_number;

    const char * host = NULL;
    const char * port = NULL;
    const char * login = NULL;
    
    // kontrola argumentov
    if(checkArguments(argc, argv, &host, &port, &login, &msg, &port_number))
        exit(1);
    
    const char *server_hostname = host;

    // pripajanie na server
    Socket_t *soc = ConnectToTCPServer(port_number, server_hostname);
    
    // posielanie tzv. CONNECT REQUEST spravy
    SendTCPMessage(soc->soc_fd, (char *)&conn_msg, sizeof(conn_msg));
    // prijmanie odpovede zo servera
    int res = RecieveTCPMessage(soc->soc_fd, (char *) &conn_msg,sizeof(conn_msg));

    // neuspesny pokus o pripojenie
    if(conn_msg.flag != CONNECT_OK || res <= 0){
        fprintf(stderr, "Unsuccesful connect");
        exit(1);
    }


    if(login){
        if(BUFSIZE <= strlen(login)) // indikuje posielanie na viackrat
            for(const char * help = login; msg.flag != DATA_END; help+=(BUFSIZE-1) ){
                memset(msg.data, '\0', BUFSIZE); // vynulovanie bufferu

                int value = BUFSIZE-1;
                if(help + (BUFSIZE-1) >= (login + strlen(login))){  // posielanie poslednej casti spravy indikuje DATA_END
                    value = (login + strlen(login))-help;
                    msg.flag = DATA_END;
                }

                strncpy(msg.data, help, value); // kopirovanie spravy
                SendTCPMessage(soc->soc_fd, (char *)&msg, sizeof(msg));
                
            }
        else{ // posielanie spravy na 1 krat 
            strncpy(msg.data, login, BUFSIZE-1); // kopirovanie dat
            SendTCPMessage(soc->soc_fd, (char *)&msg, sizeof(msg)); 
            
            msg.flag = CONFIRM_END; // indikuje koniec posielania ak ide o kratku spravu
            SendTCPMessage(soc->soc_fd, (char *)&msg, sizeof(msg));

        }
    }
    else{
        // nebol urceny parameter login
        SendTCPMessage(soc->soc_fd, (char *)&msg, sizeof(msg)); 
        
        msg.flag = CONFIRM_END; // indikuje koniec posielania ak ide o kratku spravu
        SendTCPMessage(soc->soc_fd, (char *)&msg, sizeof(msg));
 
    }

    // prijmanie dat od servera
    do{
        memset(msg.data, '\0', BUFSIZE); // vynulovanie buffera
        
        res = RecieveTCPMessage(soc->soc_fd, (char *) &msg,sizeof(msg));
        if(res <= 0)
            break;
        if(msg.flag == DATA || msg.flag == DATA_END)
            printf("%s", (char *)msg.data);
        if(msg.flag == SERVER_ERR){ // chyba na strane servera, neexustujuci login a pod.
            fprintf(stderr,"Unsuccesful request\n");
            exit(1);
;
        }
        if(res <= 0)
            break;
        
    }while (msg.flag != DATA_END);

    // close socket
    CloseSocket(soc);
    
    // free memory
    DestroySocket(soc);

    printf("\n");
    return 0;
}