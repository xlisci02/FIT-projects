/**
  htab_bucket_count.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
**/

#include "private_hash_table.h"
#include "hash_table.h"

/*
* Funkcia htab_bucket_count vracia pocet prvkov pola (arr_size) 
*/
size_t htab_bucket_count(htab_t * t)
{
	return t->arr_size;
}

