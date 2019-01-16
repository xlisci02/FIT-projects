/**
  proj2.h
  Riešenie IOS-Projekt2,  28.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#ifndef PROJ2_H
#define PROJ2_H
#define LIMIT 5001


#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h> 

// makro, vracia mimum dvoch cisel
#define MIN(number1, number2) \
	(number1 < number2)? number1 : number2

// makro, overuje ci je cislo v rozsahu <0,5000>
#define IS_IN_RANGE(number)\
	(((number >=0) && (LIMIT > number)))? 1 : 0

// struktura, do kt. sa ukladaju data zo vstupu
typedef struct 
{
	int A;
	int C;
	int AGT;
	int CGT;
	int AWT;
	int CWT;
}params_t;

/* FUNKCIE */
/* 
* Funkcia clear sluzi na:
* Uvolnenie zdielanej pamate a semaforov
* premenna err signalizuje ci sa cisti:
* z dovodu chyby vo funkcii set (1)
* inak (0)
*/
void clear(int a, int err);

/* 
* Funcia set  
* Nastavi zdroje (semafory, zdialana pamat)
*/
void set();

/* 
* Funkcia get_rand:
* Vracia pseudonahodne cislo z rozsahu <0,max>
* pozn. % max+1 = cislo z rozsahu <0,max>
* 		% max   = cislo z rozsahu <0,max-1>
*/
int get_rand(int max);

/* 
* Funkcia proc_adult 
* Synchronizacia procesu adult pomocou semaforov
* sleep_time: maximalna hodnota doby, pocas ktorej proces adult 
* simuluje svoju cinnost v centre 
*/
void proc_adult(int sleep_time);

/* 
* Funkcia proc_child 
* Synchronizacia procesu child pomocou semaforov
* sleep_time: maximalna hodnota doby, pocas ktorej proces child 
* simuluje svoju cinnost v centre 
*/
void proc_child(int sleep_time);

/* 
* Funkcia test_arguments 
* Testovanie a spracovanie argumentov
* V pripade uspechu vracia strukturu params_t s nastavenymi polozkami
* Ak nie konci program s chybovym hlasenim
*/
params_t test_arguments(int argc, char * argv[]);

/* 
* Funkcia fork_err 
* Vola funkciu clear() tak aby sa vsetko zatvorilo a zmazalo 
* Vykonava kill procesov ktorych pid su ulozene v adult_ids a child_ids
* Kolko pid procesov je ulozenych v jednotlivych poliach udavaju hodnoty adults resp. childs
*/
void fork_err(pid_t * adult_ids, pid_t * child_ids, int adults, int childs);


#endif // PROJ2_H
