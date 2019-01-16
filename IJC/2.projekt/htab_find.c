/**
  htab_find.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"
#include <string.h>

/* 
* V tabulke t vyhlada zaznam odpovedajuci retazcu key
* - ak ho najde, vrati ukazatel na zaznam
* - ako ho nenajde vracia NULL
*/
struct htab_listitem * htab_find(htab_t * t, char * key)
{
	if(t==NULL || key ==NULL)
		return NULL;

	unsigned int index = hash_function(key) % htab_bucket_count(t);

	for(struct htab_listitem * tmp = t->ptr[index];tmp!=NULL;tmp = tmp->next)
	{
		if(strcmp(tmp->key,key)==0)		
			return tmp;
	}
	return NULL;
}
