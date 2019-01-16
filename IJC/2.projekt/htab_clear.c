/**
  htab_clear.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Zrušenie všetkých položiek, tabuľka ostane prázdna
**/

#include "private_hash_table.h"
#include "hash_table.h"

/*
* Funkcia pre zrušenie všetkých položiek tabuľky (tabuľka ostane prázdna)
*/
void htab_clear(htab_t * t)
{
	if(t==NULL)
		return;
	unsigned  size =  htab_bucket_count(t); 
	struct htab_listitem * xchg = NULL;

	for(unsigned  i = 0; i < size; i++)
	{
		for(struct htab_listitem * tmp = t->ptr[i]; tmp!=NULL; tmp = xchg)
		{
			t->n--;
			xchg=tmp->next;
			free(tmp->key);
			free(tmp);
		}
		t->ptr[i]=NULL;
	}
}