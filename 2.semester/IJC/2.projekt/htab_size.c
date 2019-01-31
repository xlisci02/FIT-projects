/**
  htab_size.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "hash_table.h"
#include "private_hash_table.h"

/*
* Funkcia htab_size vracia počet prvkov tabulky (n)
*/
size_t htab_size(htab_t * t)
{
	return t->n;
}

