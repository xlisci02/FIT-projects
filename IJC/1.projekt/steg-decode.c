/**
  steg-decode.c
  Riešenie IJC-DU1, príklad b), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Funkcia na otestovanie funkcií, ktorá dekóduje správu zo súboru a vypíše ju 
**/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "error.h"
#include "ppm.h"
#include "bit_array.h"
#include "eratosthenes.h"
#define MAX 1000*1000*3

int main(int argc, char * argv[])
{
	//Chybny pocet argumentov
	if (argc != 2)
	{	
		error_msg("Zle spustenie programu\n");
		return 1;
	}

	struct ppm * p = ppm_read(argv[1]);
 	// ak ppm_read vrati NULL nastala chyba
 	if (p==NULL)
 	{
 		free(p);
 		error_msg("Chyba pri praci so suborom\n");
 		return 1;
 	}
 	
 	// Vytvorenie pola, pouzitie Eratos.sita 
	ba_create(POLE,MAX);
	Eratosthenes(POLE);
	int bit=0;
	char znak=0;

	// Nulovy bit oznacuje prvocislo, dalej sa log. AND-om odstrihne posledny bit, posunie sa a log.OR-om sa zapise hodnota bitu   
	for (unsigned long i = 2; i < MAX; i++)
	{
		if ((ba_get_bit(POLE,i)) == 0)
		{
			bit++;
			znak = (znak | ((p->data[i] & 1) << (bit-1)));			
			
			if(bit == 8)
			{	
				printf("%c",znak);
				if (znak=='\0')
					break;
				bit=0;
				znak=0;
			}
		}
		
	}
	free(p);
	//Ak posledny znak nieje '\0' -> chyba
	if(znak!='\0')
		error_msg("Retazec nieje korektne ukonceny\n");
	printf("\n");
	return 0;
}