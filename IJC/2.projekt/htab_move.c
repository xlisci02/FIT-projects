/**
  htab_move.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"

/* 
* Chova sa ako move konstruktor: vytvori a inicializuje
  tabulku s datami z tabulky t2, t2 nakoniec ostane prazdna a alokovana
*/
htab_t * htab_move(unsigned newsize,htab_t * t2)
{
	htab_t * new = htab_init(newsize);
	
	if(t2==NULL || new ==NULL)
		return NULL;
	
	struct htab_listitem * x = NULL;
	
	unsigned size = htab_bucket_count(t2);

	for(unsigned i = 0; i < size; i++)
	{
		for(struct htab_listitem * tmp = t2->ptr[i]; tmp!=NULL; tmp = tmp->next)
		{	
			x = htab_lookup_add(new,tmp->key);
			x->data = tmp->data;
		}
	}
	htab_clear(t2);
	return new;
}