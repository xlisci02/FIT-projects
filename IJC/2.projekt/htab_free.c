/**
  htab_free.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"

/* 
* Chova sa ako destruktor: zrusi tabulku
*/
void htab_free(htab_t * t)
{
	if(t==NULL)
		return;
	htab_clear(t);
	free(t);
}
