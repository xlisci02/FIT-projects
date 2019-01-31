/**
  private_hash_table.h
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Hlavickovy subor obsahujuci uplnu deklaraciu struktury htab_t
  Aby tabulka bola pre kniznicu privatna 
**/

#ifndef PRIVATE_HASH_TABLE_H
#define PRIVATE_HASH_TABLE_H

struct htab_t
{
	unsigned arr_size; // velkost pola ukazatelov
	unsigned n; // aktualny pocet zaznamov
	struct htab_listitem *ptr []; // pole ukazatelov na stukturu polozky
};


#endif // PRIVATE_HASH_TABLE_H