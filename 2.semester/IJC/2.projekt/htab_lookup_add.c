/**
  htab_lookup_add.c
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
* - ako ho nenajde automaticky prida zaznam a vrati ukazatel
*/
struct htab_listitem * htab_lookup_add(htab_t * t, char * key)
{
	if(t==NULL || key ==NULL)
		return NULL;

	struct htab_listitem * tmp = htab_find(t, key);
	if(tmp!=NULL)
	{
		tmp->data++;
		return tmp;
	}

	unsigned int index = hash_function(key) % htab_bucket_count(t);
	struct htab_listitem * new = (struct htab_listitem *)malloc(sizeof(struct htab_listitem));
	
	new->key=(char *)malloc (sizeof(char) * (strlen(key)+1));

	if(new == NULL || new->key == NULL)
		return NULL;

	strcpy(new->key,key);
	new->data=1;
	new->next=t->ptr[index];
	t->ptr[index]=new;
	t->n++;
	return new;		
}