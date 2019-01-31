// Matúš Liščinský - xlisci02 
/*******************************
 * IPS 2017 - projekt 1 - Paralelni grep
 * Subor: pgrep.c
 * Prelozene: g++ (Ubuntu/Linaro 6.3.0-18ubuntu2~16.04) 6.3.0 20170519
 * Preklad: $g++ pgrep.c -o pgrep -lpthread
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 12.10.2017
 * *****************************/

#include<stdio.h>
#include<thread>
#include<mutex>
#include<vector>
#include<iostream>
#include<regex>

std::vector<std::mutex *> zamky; /* pole zamkov premennej velkosti */
std::mutex mut, endline; /* zamky na ovladanie vlakien*/
 

int score;
int count;
char *line;

/*******************************
 * Funkcia pre citanie riadku
 * *****************************/
char *read_line(int *res) 
{
	std::string line;
	char *str;
	if (std::getline(std::cin, line)) {
		str=(char *) malloc(sizeof(char)*(line.length()+1));
		strcpy(str,line.c_str());
		*res=1;
		return str;
	} else {
		*res=0;
		return NULL;
	}
}
/*********************************************
 * Funkcia pre vlakna
 * Kazde vlakno hlada svoj regex na riadku
 * *******************************************/
void f(int i, int num, std::regex re, int sc)
{
	while(1)
	{	
		(*(zamky[i])).lock(); // kazde vlakno caka na svoj zamok
		if(!line) // ak sa nic neprecita vlakno vypadne z cyklu 
			break;
		mut.lock();
			if(std::regex_match(line,re)) // regex match
				score+=sc;

			// inkrementacia poctu skontrolovanych regexov na jednom riadku
			count++; 
			if(count==num) // mozeme ist na dalsi riadok
				endline.unlock();
		mut.unlock();
	}	
}

int main(int argc, char * argv[])
{
	int min_score; 

	/*******************************
	 * Kontrola argumentov
	 * *****************************/
	char ERR_str [] = "USAGE: pgrep MIN_SCORE RE1 SC1 [ RE2 SC2 ] [ RE3 SC3 ] ...\n";
	
	if(argc < 4 || argc % 2)
	{
		fprintf(stderr, "%s",ERR_str);
		return 1;
	}
	
	min_score = atoi(argv[1]); 
	const int num = (argc-2)/2;

	/* Zamknutie ovladacich semaforov */
	mut.lock();
	endline.lock();
	
	/* Inicializacia threadov a zamkov */
	std::vector <std::thread *> threads; /* pole threadov premennej velkosti */
	

	/* Vytvorenie zamkov */
	zamky.resize(num); /* nastavenie velkosti pola zamky */
	for(int i=0;i<num;i++){	
		std::mutex *new_zamek = new std::mutex();
		zamky[i]=new_zamek;		
		/* Zamknutie novo vytvoreneho zamku */
		(*(zamky[i])).lock();
	}
	/* Vytvorenie threadov */
	threads.resize(num); /* nastavenie velkosti pola threads */
	for(int i=0;i<num;i++){	
		int pos = (i+1)*2; //position
		std::thread *new_thread = 
			new std::thread (f,i, num, std::regex(argv[pos]),atoi(argv[pos+1]));
		threads[i]=new_thread;
	}
	
	/**********************************
	 * Vypocet pgrep
	 * ********************************/
	int res;
	line=read_line(&res); 
	while (res) {
		for(int i=0;i<num;i++)
			(*(zamky[i])).unlock();
		mut.unlock(); // mutex unlock -> thready zacinaju bezat
		endline.lock(); // koniec spracovavania riadku 
		mut.lock();
		if(score >= min_score) 
			printf("%s\n",line);

		free(line); /* uvolnenie pamate */
		score=0;
		count=0;
		line=read_line(&res); // znovu citam riadok
	}
	
	/* Na konci sa odomknu vsetky zamky */
	for(int i=0;i<num;i++)
		(*(zamky[i])).unlock();
	

	/**********************************
	 * Uvolnenie pamati
	 * ********************************/

	/* Vykona sa join a uvolni pamat threadov */
	for(int i=0;i<num;i++){
		(*(threads[i])).join();
		delete threads[i];
	}
	/* uvolnenie pamati zamkov */
	for(int i=0;i<num;i++){
		delete zamky[i];
	}
	
}