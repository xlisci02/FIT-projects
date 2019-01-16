/**
  io.h
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Hlavickovy subor pre io.c
**/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef IO_H
#define IO_H

// premenna na varovanie, ze bol prekroceny limit 
extern int overfull_war;

// Funckia get_word, ktora cita slovo zo vstupu (viac v io.c)
int get_word(char *s, int max, FILE *f);

#endif