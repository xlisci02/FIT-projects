/**
  io.c
  Riešenie IJC-DU2, príklad b), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Obsahuje funckiu get_word, ktora cita slovo zo vstupu
**/

#include "io.h"

// Premenna na varovanie pri prekroceni limitu
int overfull_war=0;

/* 
* Funkcia get_word cita jedno slovo zo suboru f do zadaneho pola znakov a vrati dlzku slova
* Ak narazi na koniec suboru funkcia vracia EOF. 
* Funckia cita slova s limitom max-1 znakov ak bude retazec dlhsi ako max vrati max-1
*/
int get_word(char *s, int max, FILE *f)
{

	if(f==NULL)
		return EOF;
	max--;
	strcpy(s,"");
	int c;
	int znak=0;

	//Preskocenie prvych medzier
	while(isspace((c=getc(f))))
	{
		znak++;
		continue;
	}
	
	// ak naratim pri preskakovani medzier na EOF, vraciam EOF
	if(c==EOF)
		return EOF;

	// jeden znak spat
	ungetc(c,f);

	// premenna na pocet nac. znakov
	znak=0;
	
	// cyklus nacitava znaky kym nenacita EOF 
	// alebo kym nacita dostatocny pocet znakov 
	while ((c=getc(f))!=EOF)
	{
		
		if (isspace(c))
		{
			return znak;
		}

		else if (znak >= max)
		{
			overfull_war=1; // WARNING 
			while((c=getc(f)))
			{
				if(isspace(c))
					return znak;
				if(c==EOF)
					return EOF;
			}	
		}
		else 
		{
			s[znak]=c;
			s[znak+1]='\0';
			znak++;
		}

	}
	return EOF;
}