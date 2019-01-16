/**
  proj2.c
  Riešenie IOS-Projekt2,  28.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "proj2.h"
#define shmSIZE sizeof(int)

// GLOBALNA UROVEN

FILE * file;

/* SEMAFORY */

// semafor pre kriticku sekciu - mutex
sem_t *mutex;

// semafor pre adults ktore chcu odist z centra
sem_t * adult_front;

// semafor pre childs ktore chcu vstupit do centra
sem_t * child_front; 

// semafor aby childs aj adults skoncili sucasne
sem_t * FINISH;

/* ZDIELANE PREMENNE */
// Poradove cislo akcie
int P_id ; 
int * P;

// Pocet vygenerovanych Adults
int A_id ;
int * A;

// Pocet vygenerovanych Childs
int C_id ;
int * C ;

// Pocet adults v centre
int A_in_centre_id ;
int * A_in_centre ;

// Pocet childs v centre
int C_in_centre_id;
int * C_in_centre ;

// Pocet childs cakajucich na vstup do centra
int C_waiting_id ;
int * C_waiting;

// Pocitadlo vygenerovanych procesov Adults + Childs
int processes_id;
int * processes ;

// Pocet adults, ktore chchu opustit centrum
int A_leaving_id;
int * A_leaving ;

// Pocet adults, ktore este pridu do centra 
int A_coming_id;
int * A_coming ;

/* FUNKCIE */
/* 
* Funkcia clear sluzi na:
* Uvolnenie zdielanej pamate a semaforov
* premenna err signalizuje ci sa cisti:
* z dovodu chyby vo funkcii set (1)
* inak (0)
*/
void clear(int a, int err)
{ 
	// bez breaku prechadza do dalsich 'case'
	// samozrejme v opacnom poradi ako pri vytvarani vo funckii set
	switch (a)
	{
		case 22: munmap(A_coming, shmSIZE); 
		case 21: munmap(A_leaving, shmSIZE); 
		case 20: munmap(processes, shmSIZE); 
		case 19: munmap(C_waiting, shmSIZE); 
		case 18: munmap(C_in_centre, shmSIZE); 
		case 17: munmap(A_in_centre, shmSIZE); 
		case 16: munmap(C, shmSIZE); 
		case 15: munmap(A, shmSIZE); 
		case 14: munmap(P, shmSIZE); 
		case 13: shm_unlink("/xlisci02_a_coming"); 
		case 12: shm_unlink("/xlisci02_a_leaving");
		case 11: shm_unlink("/xlisci02_proc");
		case 10: shm_unlink("/xlisci02_c_wait");
		case 9: shm_unlink("/xlisci02_c_centre");
		case 8: shm_unlink("/xlisci02_a_centre");
		case 7: shm_unlink("/xlisci02_cid");
		case 6: shm_unlink("/xlisci02_aid");
		case 5: shm_unlink("/xlisci02_pid");
		case 4: sem_close(FINISH); sem_unlink("/xlisci02_finish");
		case 3: sem_close(child_front); sem_unlink("/xlisci02_cf");
		case 2: sem_close(adult_front); sem_unlink("/xlisci02_af");
		case 1: sem_close(mutex); sem_unlink("/xlisci02_mutex");
		case 0: break;
	}
	if(err)
	// Vypis v pripade chyby v set()
	{
		if(a < 3)
			fprintf(stderr, "[CHYBA] Nepodarilo sa otvorit semafor \n" );
		else 
			fprintf(stderr, "[CHYBA] Pri praci so zdielanou pamatou \n" );
		exit(2);
	}	
}	

/* 
* Funcia set  
* Nastavi zdroje (semafory, zdialana pamat)
*/
void set()
{
// SEMAFORY (ich popis viz. vyssie)

if 	((mutex = sem_open("/xlisci02_mutex", O_RDWR | O_CREAT,0666,1)) == SEM_FAILED)		{clear(0,1);} 
if 	((adult_front = sem_open("/xlisci02_af", O_RDWR | O_CREAT,0666,0)) == SEM_FAILED)	{clear(1,1);}
if 	((child_front = sem_open("/xlisci02_cf", O_RDWR | O_CREAT,0666,0)) == SEM_FAILED) 	{clear(2,1);}
if 	((FINISH = sem_open("/xlisci02_finish", O_RDWR | O_CREAT,0666,0)) == SEM_FAILED)	{clear(3,1);}

//ZDIELANE PREMENNE (ich popis viz. vyssie)

if  ((P_id = shm_open("/xlisci02_pid",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 ) 					{clear(4,1);}	ftruncate(P_id, shmSIZE);
if	((A_id = shm_open("/xlisci02_aid",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 ) 					{clear(5,1);}	ftruncate(A_id, shmSIZE);
if	((C_id = shm_open("/xlisci02_cid",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 )					{clear(6,1);}	ftruncate(C_id, shmSIZE);
if	((A_in_centre_id = shm_open("/xlisci02_a_centre",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 ) 	{clear(7,1);}	ftruncate(A_in_centre_id, shmSIZE);
if	((C_in_centre_id = shm_open("/xlisci02_c_centre",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 ) 	{clear(8,1);}	ftruncate(C_in_centre_id, shmSIZE);
if	((C_waiting_id = shm_open("/xlisci02_c_wait",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 ) 		{clear(9,1);}	ftruncate(C_waiting_id, shmSIZE);
if	((processes_id = shm_open("/xlisci02_proc",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 )			{clear(10,1);}	ftruncate(processes_id, shmSIZE);
if	((A_leaving_id = shm_open("/xlisci02_a_leaving",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 )	{clear(11,1);}	ftruncate(A_leaving_id, shmSIZE);
if	((A_coming_id = shm_open("/xlisci02_a_coming",O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) < 0 )		{clear(12,1);}	ftruncate(A_coming_id, shmSIZE);

if	((P = (int*) mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, P_id, 0))  == MAP_FAILED ) 					{clear(13,1);}	close(P_id);
if	((A = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, A_id, 0)) == MAP_FAILED ) 						{clear(14,1);}	close(A_id);
if	((C = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, C_id, 0)) == MAP_FAILED ) 						{clear(15,1);}	close(C_id);
if	((A_in_centre = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, A_in_centre_id, 0)) == MAP_FAILED ) 	{clear(16,1);}	close(A_in_centre_id);
if	((C_in_centre = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, C_in_centre_id, 0)) == MAP_FAILED ) 	{clear(17,1);}	close(C_in_centre_id);
if	((C_waiting= (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, C_waiting_id, 0)) == MAP_FAILED ) 		{clear(18,1);}	close(C_waiting_id);
if	((processes = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, processes_id, 0)) == MAP_FAILED ) 		{clear(19,1);}	close(processes_id);
if	((A_leaving = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, A_leaving_id, 0)) == MAP_FAILED ) 		{clear(20,1);}	close(A_leaving_id);
if	((A_coming = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, A_coming_id, 0)) == MAP_FAILED )			{clear(21,1);}	close(A_coming_id);

}   

/* 
* Funkcia get_rand:
* Vracia pseudonahodne cislo z rozsahu <0,max>
* pozn. % max+1 = cislo z rozsahu <0,max>
* 		% max   = cislo z rozsahu <0,max-1>
*/
int get_rand(int max)
{
	if(!max)
		return 0;
	srand((unsigned) time (NULL));
	return rand() % max+1;
}

/* 
* Funkcia proc_adult 
* Synchronizacia procesu adult pomocou semaforov
* sleep_time: maximalna hodnota doby, pocas ktorej proces adult 
* simuluje svoju cinnost v centre 
*/
void proc_adult(int sleep_time)
{
	sem_wait(mutex);
		int ID =++(*A); // jedinecne ID procesu
		fprintf(file,"%-8d: A %-4d: started\n",(*P)++,ID); // proces zacal
		
	sem_post(mutex);

	sem_wait(mutex);
		fprintf(file,"%-8d: A %-4d: enter\n",(*P)++,ID); // proces vstupil do centra
		
		(*A_in_centre)++; // pocet adults v centre sa inkrementuje
		(*A_coming)--;    // pocet adults ktory sa chystaju prist do centra sa dekrementuje
		if((*C_waiting))  // ak childs cakaju pusti ich, maximalne 3
		{	
			int n = MIN(3,(*C_waiting)); // n = kolko childs pusti, pouziva makro MIN
			for(int i=0; i < n; i++)
				sem_post(child_front); // povolenie semaforu child_front -> na ktory childs cakaju ak su v stave waiting
			((*C_waiting))-=n;  // pocet cakajucich deti sa znizi o n
			(*C_in_centre)+=n;  // pocet deti v centre sa zvysi o n
		
		}	
	sem_post(mutex);
	
	/*	
	*	simulacia cinnosti v centre
	*  	funkcia usleep vyzaduje parameter v mikrosekundach
	*	uzivatel zadal dobu v milisekundach, preto x 1000 
	*/
	usleep(get_rand(sleep_time) * 1000); 
	
	sem_wait(mutex);
	
		fprintf(file,"%-8d: A %-4d: trying to leave\n",(*P)++,ID); // adult chce odist 
		

		if((*C_in_centre) <= 3*((*A_in_centre)-1)) // ak bude platit zakladna podmienka aj ked adult odide
		{
			fprintf(file,"%-8d: A %-4d: leave\n",(*P)++,ID); // adult odchadza
			
			(*A_in_centre)--; // pocet adults sa dekrementuje 
			(*processes)--; // pocitadlo procesov tiez
			sem_post(mutex);
		}
		else	// ak by neplatila zakl. podmienka centra, adult caka 
		{
			fprintf(file,"%-8d: A %-4d: waiting : %-2d: %-2d\n",(*P)++,ID,(*A_in_centre),(*C_in_centre));
			
			(*A_leaving)++;
			sem_post(mutex);
			sem_wait(adult_front);	

			sem_wait(mutex);
				fprintf(file,"%-8d: A %-4d: leave\n",(*P)++,ID); // adult odchadza
				(*processes)--; // pocitadlo procesov sa dekrementuje
			sem_post(mutex);
		}

	// Ak uz ziaden rodic nepride a v centre ziaden nieje, pustia sa vsetci cakajuce childs procesy
	if((*A_coming)==0 && (*A_in_centre)==0)
		for(int i=0; i < (*C_waiting); i++)
			sem_post(child_front);
	
	// Podmienka plati v pripade ze to nieje posledny vygenerovany proces
	if((*processes))
	{		
		sem_wait(FINISH); // procesy ostanu cakat kym im posledny proces povoli semafor FINISH
		sem_wait(mutex);
		fprintf(file,"%-8d: A %-4d: finished\n",(*P)++,ID); // procesy finishuju 
		sem_post(mutex);
	}
	else  // posledny proces
	{	
		for(int i=0; i < ((*A)+(*C)-1);i++) // povoli semafor FINISH pre vsetky ostatne procesy -> mozu finishovat
			sem_post(FINISH);
		sem_wait(mutex);
		fprintf(file,"%-8d: A %-4d: finished\n",(*P)++,ID); // s nimi finishuje sucasne aj on
		sem_post(mutex);
	}

}

/* 
* Funkcia proc_child 
* Synchronizacia procesu child pomocou semaforov
* sleep_time: maximalna hodnota doby, pocas ktorej proces child 
* simuluje svoju cinnost v centre 
*/
void proc_child(int sleep_time)
{
	sem_wait(mutex);	 
		int ID = ++(*C);  // jedinecne ID procesu
		fprintf(file,"%-8d: C %-4d: started\n",(*P)++,ID); // proces zacal
		
	sem_post(mutex);

	sem_wait(mutex);
	
		if((*C_in_centre) < 3* (*A_in_centre) || 
		((*A_coming)==0 && (*A_in_centre)==0)) // test zakladnej podmienky resp. ak uz ziaden rodic nepride ani v centre nieje, moze vstupit
		{	
			fprintf(file,"%-8d: C %-4d: enter\n",(*P)++,ID);
			(*C_in_centre)++;
			sem_post(mutex);
		}

		else	// ak by bola porusena podmienka musi cakat 
		{
			fprintf(file,"%-8d: C %-4d: waiting : %-2d: %-2d\n",(*P)++,ID,(*A_in_centre),(*C_in_centre));
			
			(*C_waiting)++;
			sem_post(mutex);
			
			sem_wait(child_front);	// caka na prichod rodica
			sem_wait(mutex);
				fprintf(file,"%-8d: C %-4d: enter\n",(*P)++,ID); // child vstupuje do centra

			sem_post(mutex);
		
	}

	// simulacia cinnosti procesu child v centre
	usleep(get_rand(sleep_time) * 1000);
	
	sem_wait(mutex);
		fprintf(file,"%-8d: C %-4d: trying to leave\n",(*P)++,ID); // child ochadza z centra
		
		fprintf(file,"%-8d: C %-4d: leave\n",(*P)++,ID);
		
		(*processes)--;		// dekrementuj pocitadlo procesov
		(*C_in_centre)--;   // dek. pocet deti v centre
	
	
		if((*A_leaving) && ((*C_in_centre) <= 3*((*A_in_centre)-1))) // ak nejaky adult chce odist a nebude porusena podmienka odide
		{
			(*A_in_centre)--;
			(*A_leaving)--;
			sem_post(adult_front);		
		}
	sem_post(mutex);

	// finishing procesov obdobne ako v proc_adult
	// vsetky procesy cakaju na posledneho ktory povoli semafor a ukoncia sa sucasne
	if((*processes))
	{		
		sem_wait(FINISH);
		sem_wait(mutex);
			fprintf(file,"%-8d: C %-4d: finished\n",(*P)++,ID);
			
		sem_post(mutex);
	}
	else
	{	
		for(int i=0; i < ((*A)+(*C)-1);i++)
			sem_post(FINISH);
		sem_wait(mutex);
			fprintf(file,"%-8d: C %-4d: finished\n",(*P)++,ID);
			
		sem_post(mutex);
	}

}

/* 
* Funkcia test_arguments 
* Testovanie a spracovanie argumentov
* V pripade uspechu vracia strukturu params_t s nastavenymi polozkami
* Ak nie konci program s chybovym hlasenim
*/
params_t test_arguments(int argc, char * argv[])
{
	params_t tab;
	if(argc != 7) // 6 argumentov + argv[0]
	{	
		fprintf(stderr, "Chybny pocet argumentov\n");
		exit(1);
	}
	char * ptr = NULL;

	for(int i=1; i < argc; i++)
	{	
		long num = (int)strtol(argv[i],&ptr,10);
		
		if(i > 2)
		{
			if(*ptr!='\0' || !IS_IN_RANGE(num)) // pre polozky AGT,CGT,AWT,CWT ; pouziva makro IS_IN_RANGE
			{	
				fprintf(stderr, "Spustenie so zlymi parametrami\n");
				exit(1);
			}
		}
		else 
		{	
			if(*ptr!='\0' || num <=0 )	// pre polozky A,C
			{	
				fprintf(stderr, "Spustenie so zlymi parametrami\n");
				exit(1);
			}
		}
		switch (i){
		case 1: tab.A = num; break;
		case 2: tab.C = num; break;
		case 3: tab.AGT = num; break;
		case 4: tab.CGT = num; break;
		case 5:	tab.AWT = num; break;
		case 6: tab.CWT = num; break;
		}
	}
	return tab; 
}

/* 
* Funkcia fork_err 
* Vola funkciu clear() na zrusenie zdrojov
* Vykonava kill procesov ktorych pid su ulozene v adult_ids a child_ids
* Kolko pid procesov je ulozenych v jednotlivych poliach udavaju hodnoty adults resp. childs
*/
void fork_err(pid_t * adult_ids, pid_t * child_ids, int adults, int childs)
{
	clear(22,0);
	
	for(int a = 0; a < adults; a++)
		kill(adult_ids[a],SIGTERM);

	for(int c = 0; c < childs; c++)
		kill(child_ids[c],SIGTERM);
}

/* 
* Funkcia main
*/
int main(int argc, char * argv [])
{
	// Otvorenie suboru	
	file = fopen("proj2.out","w");
	if (file == NULL)
	{	
		fprintf(stderr, "[CHYBA] Nepodarilo sa otvorit subor na zapis\n");
		return 2;
	}	
	setbuf(file,NULL);

	//Spracovanie argumentov
	params_t tab = test_arguments(argc, argv);

	setbuf(stdout,NULL);
	setbuf(stderr,NULL);

	// set, nast. zdrojov
	set();

	// Alokovanie priestoru pre ukladanie pid procesov 
	pid_t * adult_ids= (pid_t *)malloc(sizeof(pid_t)*tab.A);	
	pid_t * child_ids= (pid_t *)malloc(sizeof(pid_t)*tab.C);

	// Pocitadlo akcii nast. na 1
	(*P)=1;
	(*A_coming)=tab.A; 
	(*processes)=tab.A+tab.C;

	// 1.fork
	pid_t pid = fork();
	
	// testovanie forku
	if(pid < 0)
	{
		fprintf(stderr,"Fork fail\n");
		fork_err(adult_ids,child_ids,0,0);
		free(adult_ids);
		free(child_ids);
		fclose(file);
		return 2;
	}

	// pomocny proces pre generovanie deti
	else if(pid == 0)
	{ 
		for(int i=0; i < tab.C; i++)
		{	
			usleep(get_rand(tab.CGT) * 1000); 
			
			if((child_ids[i] = fork()) < 0) //test fork, cyklicky generuje childs
			{
				fprintf(stderr,"Fork fail\n");
				fork_err(adult_ids,child_ids,0,i);
				free(child_ids);
				fclose(file);
				return 2;
			}
			else if(child_ids[i] ==0)
			{
				proc_child(tab.CWT);
				exit(0);
			}
				
		}
		
		for(int i=0; i < tab.C; i++) // caka kym vsetky childs skoncia az potom ukonci pomocny proces ktory generuje childs
				wait(NULL);
		
		return 0;
	}
	else
	{
		
		pid = fork();
		
		if(pid < 0) // test fork
		{
			fprintf(stderr,"Fork fail\n");
			fork_err(adult_ids,child_ids,0,0);
			free(adult_ids);
			fclose(file);
			return 2;
		}
		else if(pid == 0)
		{
			// pomocny proces na generovanie adults
			for(int i=0; i < tab.A; i++)
			{	
				usleep(get_rand(tab.AGT) * 1000);
				
				if((adult_ids[i] = fork()) < 0) //test fork, cyklicky generuje adults
				{
					fprintf(stderr,"Fork fail\n");
					fork_err(adult_ids,child_ids,i,0);
					free(adult_ids);
					fclose(file);
					return 2;
				}
				else if(adult_ids[i] == 0)
				{
					proc_adult(tab.AWT); 
					exit(0);
				}
				
			}

		for(int i=0; i < tab.A; i++) // caka kym vsetky procesy adults skoncia az potom sa ukonci tento pomoc. proces
			wait(NULL);
		return 0;
		}
	}
	// hlavny proces caka na ukoncenie svojich potomkov 
	wait(NULL);
	wait(NULL);
		
	// uvolnenie pamate 
	free(adult_ids); 
	free(child_ids);
	fclose(file);
	
	// zrusenie zdrojov
	clear(22,0);
	return 0;

}