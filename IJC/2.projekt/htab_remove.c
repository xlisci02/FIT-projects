/**
  htab_remove.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"
#include <stdbool.h>
#include <string.h>

/* 
* Vyhlada a zrusi zadanu polozku v tabulke t 
* vracia false ak ju nenajde 
*/
bool htab_remove(htab_t * t, char * key)
{
	if(t==NULL || key ==NULL)
		return false;

	unsigned int index = hash_function(key) % htab_bucket_count(t);

	struct htab_listitem * prev = NULL;

	for(struct htab_listitem * tmp = t->ptr[index];tmp!=NULL;tmp = tmp->next)
	{
		if(strcmp(tmp->key,key)==0)
		{
			
			//Ak je to prvy prvok pola
			if(tmp==t->ptr[index])
				t->ptr[index]=tmp->next;
			else 
				prev->next = tmp->next;
			
			t->n--;
			free(tmp->key);
			free(tmp);
			return true;
		}
		prev = tmp;
	}
	return false;
}