/**
  hash_function.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Hashovacia funkcia
**/

#include "private_hash_table.h"
#include "hash_table.h"

/*
* Hash funkcia
* Rozptylovacia funkcia pre retazce podla literatury
*/
unsigned int hash_function(const char *str) 
{
	unsigned int h=0; 
	const unsigned char *p; 
	for(p=(const unsigned char*)str; *p!='\0'; p++) 
		h = 65599*h + *p; 
	return h; 
}