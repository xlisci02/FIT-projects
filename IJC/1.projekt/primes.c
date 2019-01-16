/**
  primes.c
  Riešenie IJC-DU1, príklad a), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Použitie funkcií z "bit_array.h" a "eratosthenes.h"
  na výpis posledných 10 prvočísel
**/

#include <stdio.h>
#include <stdlib.h>
#include "bit_array.h"
#include "eratosthenes.h"
 
 int main(void)
 {
 	ba_create(pole,303000000);
	Eratosthenes(pole);
	unsigned long FINAL[10];
	unsigned long i,j=0;
	for ( i=(ba_size(pole)-1); j!=10; i--)
  	{
      if ((ba_get_bit(pole,i)) == 0)
      {
        FINAL[j]=i;
        j++;
      }
 	}

 	for(int i=9; i>=0; i--)
 	{
 		printf("%lu\n", FINAL[i]);
 	}
 	return 0;
 }

