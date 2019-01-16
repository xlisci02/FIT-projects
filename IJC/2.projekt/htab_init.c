/**
  htab_init.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"

/* 
* Chova sa ako konstruktor: vytvori a inicializuje tabulku 
*/
htab_t * htab_init(unsigned size)
{
	htab_t * htab = (htab_t *) malloc(sizeof(htab) + size * sizeof(struct htab_listitem *) );
	if (htab==NULL)
		return NULL;

	htab->arr_size=size;
	htab->n=0;
	for(unsigned i=0; i < size; i++)
		htab->ptr[i]=NULL;
	return htab;	
}
