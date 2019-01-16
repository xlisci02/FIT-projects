/**
  htab_foreach.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"

/* 
* Vola zadanu funkciu pre kazdy prvok tabulky
* Obsah tabulky nemeni
*/
void htab_foreach(htab_t * t,void (*func)(char *, unsigned ))
{
	if(t==NULL)
		return;

	unsigned size = htab_bucket_count(t);
	for(unsigned i = 0; i < size; i++)
	{
		for(struct htab_listitem * tmp = t->ptr[i]; tmp!=NULL; tmp = tmp->next)
			func(tmp->key,tmp->data);
	}
}
