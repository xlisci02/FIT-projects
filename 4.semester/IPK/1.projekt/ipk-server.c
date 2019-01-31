/************************************
 * IPK 2018 - 1.Projekt 
 * Popis: Server pro ziskani informace o uzivatelich
 * File: ipk-server.c
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
 * Spustenie servera na konkretnom porte
 */
Socket_t * StartTCPServer(int port){

	// Vytvaranie socketu
	Socket_t *s = initSocket();

	if ((s->soc_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr,"ERROR: socket");
		exit(1);
	}

	// nastavenie atributov socketu
	s->addr.sin_family = AF_INET;
	s->addr.sin_addr.s_addr = INADDR_ANY;
	s->addr.sin_port = htons(port); // konvert na network byte order
	
	// bind
	if ((bind(s->soc_fd, (struct sockaddr*)&(s->addr), sizeof(s->addr))) < 0)
	{
		fprintf(stderr,"ERROR: bind");
		exit(1);		
	}
	// naslucha pre pripojenia na socket 
	// podla tohto fora maximalna hodnota backlog-u pouzivana vacsinou systemov je 128
	// https://stackoverflow.com/questions/10002868/what-value-of-backlog-should-i-use 
	if ((listen(s->soc_fd, 128)) < 0)
	{
		fprintf(stderr,"ERROR: listen");
		exit(1);				
	}
	return s;
        
}

// prijatie pripojenia na socket
Socket_t * AcceptTCPConnection(Socket_t * server_soc){
	int comm_socket = accept(server_soc->soc_fd, (struct sockaddr*)0, (socklen_t *)0);		
	if(comm_socket > 0){
		Socket_t *s = initSocket();
		s->soc_fd = comm_socket;
		return s;
		
	}
	else 
		return NULL;
}

/*
 * Pomocna funkcia, vracia 1 ak <pref> je prefixom stringu <str>
 */
int isPrefix(const char *pref, const char *str)
{
    return strncmp(pref, str, strlen(pref)) == 0;
}

/*
 * Funkcia v pripade uspechu vracia retazec s User Infom uzivatela <username> z /etc/passwd, inak NULL
 */
char * UserInfo(char * username){
	struct passwd *pw;
	if((pw = getpwnam(username)) == NULL)	
		return NULL;
	
	char * name = (char *) calloc((strlen(pw->pw_gecos)+1), sizeof(char));
	if(!name){
		fprintf(stderr,"ERR: calloc\n");
		exit(1);
	}
	strcpy(name, pw->pw_gecos);
	return name;
}

/*
 * Funkcia v pripade uspechu vracia retazec s domovskym adresarom uzivatela <username> z /etc/passwd, inak NULL
 */
char * HomeDir(char * username){
	struct passwd *pw;

	if((pw = getpwnam(username))==NULL)
		return NULL;
	
	char * dir = (char *) calloc((strlen(pw->pw_dir)+1), sizeof(char));
	if(!dir){
		fprintf(stderr,"ERR: calloc\n");
		exit(1);
	}
	strcpy(dir, pw->pw_dir);
	return dir;
}

/*
 * Funkcia v pripade uspechu vracia zoznam mien s prefixom <username> z /etc/passwd, inak NULL
 */
char * UsersList(char * username){
	struct passwd *pw;

	setpwent();
	unsigned act_size = BUFSIZE;
	char * list = (char *) calloc(act_size, sizeof(char));
	if(!list){
		fprintf(stderr,"ERR: calloc\n");
		return NULL;
	}

	while((pw = getpwent())){
		if(isPrefix(username, pw->pw_name)){

			if((strlen(list) + strlen(pw->pw_name) + 2) > act_size){
				act_size *= 2;
				list = (char *) realloc(list, act_size);
				if(!list){
					fprintf(stderr, "ERR: relloc\n");
					free(list);
					return NULL;
				}
			}
			strcat(list, pw->pw_name);
			strcat(list, "\n");
		}
	}
	
	endpwent();
	
	if(strlen(list) != 0)
		list[strlen(list)-1]='\0'; // odstranenie posledneho odriadkovania
	else{            // nenaslo sa nic 
		free(list);
		return NULL;
	}
	return list;
}


int main (int argc, const char * argv[]) {
	// osetrenie argumentov 

    char *host = NULL;
	if(argc!=3){
		fprintf(stderr, "Usage: ./ipk-server -p <port>\n");
		return 1;
	}
	int c;
	while ((c = getopt (argc,(char * const*)argv, "p:")) != -1){
	    switch (c){
	      case 'p':
	        host = optarg;
	        break;
	      case '?':
      	  default: fprintf(stderr, "Usage: ./ipk-server -p <port>\n" );
        			return 1; 
        }
    }    
	int port_number = CheckNum(host);
	if((port_number < 0 || port_number > 65535)){ 
		fprintf(stderr, "ERR: port out of range\n");
		return 1;
	} 

    
    // spusti server na porte <port_number>
    Socket_t * server = StartTCPServer(port_number);
    Socket_t * comm_socket = NULL;
	char * string = strdup("xlogin00"); // string pre prijatie spravy
	unsigned act_size = strlen(string)+1; // akt. velkost alokovaneho miesta

    Message_t  msg = { DATA,{'\0'}}; 
	int type; 
	pid_t pid;
	while(1)
	{
		type = 0;
		if((comm_socket = AcceptTCPConnection(server))) // prijatie pripojenia
		{
			if((pid = fork()) > 0){ // hlavny proces iba prijma pripojenia, detsky ich spracuva
				usleep(5000); // uspanie hlavneho procesu na 5000 us, zmiernuje vytazenie CPU
				// close socket
				CloseSocket(comm_socket);
				continue;
			}
			else if(pid < 0){
				fprintf(stderr, "Fork failed\n");
				exit(1);
			}
			else{
				int res = 0;
				// prijatie prvej spravy -- poziadavky na pripojenie
				res = RecieveTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));
				if(res == 0)
					break;
				if(res < 0){
	   					fprintf(stderr, "ERR: recv\n");
	   					exit(1);
	   			}
				if(msg.flag == CONNECT_REQ){ // schvalenie pripojenia
					msg.flag = CONNECT_OK;
	   				SendTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));
				}
				else{
					msg.flag = CONNECT_FAIL; // zamietnutie pripojenia
					SendTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));
					fprintf(stderr, "Unsuccessful connect request\n");
					exit(1);
				}

				memset(string, '\0', act_size); // vynulovanie stringu

	            do{
	                memset(msg.data, '\0', BUFSIZE); // vynulovanie buffera
	   				res = RecieveTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));
	   				if(res == 0) // chyba alebo sa client odpojil
	   					break;
	   				if(res < 0){
	   					fprintf(stderr, "ERR: recv\n");
	   					exit(1);
	   				}

	   				if(msg.flag == CONFIRM_END) // koniec spravy, uz bez uzitocnych dat
	   					break;
	   				if(!type) // typ sa nastavi podla prvej prijatej zo vsetkych prijatych casti spravy
	   					type = msg.flag;
	   				if((1 + strlen(string) + strlen(msg.data)) > act_size){
	   					act_size *= 2;
						string = (char *) realloc(string, act_size); // realloc
	   				}
	   				// konkatenacia dat ku stringu
	   				strcat(string, msg.data);
	   										
	    		}while (msg.flag != DATA_END);   		

	   			if(res == 0) // chyba alebo sa client odpojil
	   				break;
	   			msg.flag = type; // nastavenie typu 
	   			char * tmp = NULL;
	   			// obsluha poziadavky
	   			switch (msg.flag){
	   				case HOME_DIR: tmp = HomeDir(string);break;
	   				case USER_INFO: tmp = UserInfo(string);break;
	   				case LIST: tmp = UsersList(string);break;
	   				default: break;
	   			}
	   				
	   			// doslo k chybe pri obsluhe poziadavky 
	   			if(tmp == NULL){
	   				msg.flag = SERVER_ERR;
	   				SendTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));
	   				fprintf(stderr, "Something went wrong. ERR message was sent to client.\n");	
	   			}
	   				
	   			else{
	   				if(BUFSIZE <= strlen(tmp)){ // indikuje posielanie na viackrat
			  			for(char * help = tmp; msg.flag != DATA_END; help+=(BUFSIZE-1) ){
			        	        memset(msg.data, '\0', BUFSIZE); // nulovanie buffera

			   					int value = BUFSIZE-1;
			   					msg.flag = DATA; // Posielaju sa DATA
			   					if(help + (BUFSIZE-1) >= (tmp + strlen(tmp))){ // indikuje poslednu cast posielanu klientovi
			   						value = (tmp + strlen(tmp))-help;
			   						msg.flag = DATA_END;
			   					}

			   					strncpy(msg.data, help, value); // kopirovanie dat
								SendTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));
			   				}
			   			}
			   			else{  // posielanie na 1 krat 
			   				strncpy(msg.data, tmp, BUFSIZE-1); // kopirovanie dat
				            msg.flag = DATA_END; // prva zaroven poslendna cast spravy
				            SendTCPMessage(comm_socket->soc_fd, (char *)&msg, sizeof(msg));			   			}
	   			}
	   				
	   			if(tmp)
					free(tmp);
				// close socket
				CloseSocket(comm_socket);
				exit(0);
			}
		}

		// free memory
		DestroySocket(comm_socket);
	}	
	DestroySocket(server);
	if(string)
		free(string);
	CloseSocket(server);
}
