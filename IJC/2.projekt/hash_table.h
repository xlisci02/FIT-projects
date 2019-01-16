/**
  hash_table.h
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

struct htab_listitem
{
	char * key;
	unsigned data;
	struct htab_listitem * next;
};

// neuplna deklaracia struktury htab_t 
struct htab_t;
typedef struct htab_t htab_t;


//konstruktor: vytvorenie a inicializacia tabulky
htab_t * htab_init(unsigned size);

//move konstruktor: vytvorenie a inicializacia tabulky dat z tabulky t2, t2 ostane prazdna a alokovana
htab_t * htab_move(unsigned newsize,htab_t * t2);

//vrati pocet prvkov tabulky (n) 
size_t htab_size(htab_t * t); 

//vrati pocet prvkov pole (arr_size) 
size_t htab_bucket_count(htab_t * t);

//Vyhladavanie zaznamu v tabulke, ak nenajde vytvori ho
struct htab_listitem * htab_lookup_add(htab_t * t, char * key);

//Vyhladavanie zaznamu v tabulke, ak nenajde vrati NULL
struct htab_listitem * htab_find(htab_t * t, char * key);

//volanie func pre kazdy prvok tabulky
void htab_foreach(htab_t * t,void (*func)(char *, unsigned));

// vyhladavanie a zrusenie zadanej polozky, ak nenajde vracia false
bool htab_remove(htab_t * t, char * key);

//zrusenie vsetkych poloziek, tabulka ostane prazdna
void htab_clear(htab_t * t);

//destruktor: zrusenie tabulky 
void htab_free(htab_t * t);

//hashovacia funkcia
unsigned int hash_function(const char *str); 

#endif // HASH_TABLE_H