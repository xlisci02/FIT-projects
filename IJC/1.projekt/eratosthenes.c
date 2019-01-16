/**
  eratosthenes.c
  Riešenie IJC-DU1, príklad a), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Implementacia funkcie Eratosthenovho sita
**/

#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include "bit_array.h"
#include "eratosthenes.h"

void Eratosthenes (bit_array_t arr)
{

  unsigned long size = ba_size(arr);
  unsigned long i, j;
  
  ba_set_bit(arr,0,1);
  ba_set_bit(arr,1,1);

  for (i = 2; i < sqrt((double)(size)); i++) 
  {
      if ((ba_get_bit(arr,i)) == 1) continue;
        for(j = 2*i; j < size; j += i)
          ba_set_bit(arr,j,1);
  }
}


