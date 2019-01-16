/**
  ppm.h
  Riešenie IJC-DU1, príklad a), 26.3.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: gcc 5.4.0 (merlin)
  Implementácia funckií na prácu so štruktúrou ppm 
  * Čítanie zo súboru a zápis do štruktúry
  * Prepis zo štruktúry do ppm súboru
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "error.h"

struct ppm * ppm_read(const char * filename)
{
	//struct ppm * pstruct=NULL;
	
	// Otvaranie suboru s kontrolou
	FILE * file;
	file = fopen(filename, "r");
	if (file == NULL)
	{	
		warning_msg("Neporadrilo sa otvorit subor %s \n",filename);
		return NULL;
	}
	unsigned xsize;
	unsigned ysize;
	int color;
	int chyba=0;
	char a,b;

	// Kontrola hlavicky suboru
	if( fscanf(file,"%c%c ",&a,&b)!=2 || a!='P' || b!='6')
		chyba=1;
	else if( fscanf(file,"%u %u ",&xsize,&ysize)!=2  )
		chyba=1;
	else if( fscanf(file,"%d ",&color)!=1)
		chyba=1;
	if(chyba)
	{	
		warning_msg("Chybny format suboru %s \n",filename);
		return NULL;
	}

	unsigned long bin_size= 3 * xsize * ysize;
	// Alokovanie miesta pre strukturu
	// Pretypovanie (malloc vracia void pointer)
	struct ppm *pstruct = (struct ppm *) malloc( ( sizeof(struct ppm)) + (bin_size * ( sizeof(char))));

	// Kontrola alokacie struktury
	if(pstruct==NULL)
	{
		free(pstruct);
		if(fclose(file) == EOF) 
			warning_msg("Nepodarilo sa zavriet subor\n");	 
		warning_msg("Nedostatok pamate\n");
		return NULL;
	}

	else if( color < 0 || color > 255 )
	{
		free(pstruct);
		if(fclose(file) == EOF) 
			warning_msg("Nepodarilo sa zavriet subor \n");	 
		warning_msg("Zly rozsah farieb\n");		
		return NULL;
	}
	// Zapis bin. dat do struktury 

	pstruct->xsize = xsize;	
	pstruct->ysize = ysize;
//	for (int i=0; !feof(file); i++)
//		pstruct->data[i]=fgetc(file);
	
	//Citanie bin.dat zo suboru a ich zapis do struktury 
	if (fread(pstruct->data, sizeof(char), bin_size, file) != bin_size) 
    {
    	free(pstruct);
    	warning_msg("Nepodarilo sa nacitat vsetky bin.data zo suboru \n");
    	if(fclose(file) == EOF) 
			warning_msg("Nepodarilo sa zavriet subor \n");
		return NULL;
    }
	if(fclose(file) == EOF) 
			warning_msg("Nepodarilo sa zavriet subor \n");
	
	return pstruct;
}	


int ppm_write(struct ppm *p, const char * filename)
{
		
	FILE * file = fopen(filename, "w");
    unsigned long bin_size = 3 * p->xsize * p->ysize;
    
    // Kontola ci sa otvorenie podarilo
    if (file == NULL) 
    {
		warning_msg("Neporadrilo sa otvorit subor %s \n",filename);
		return -1;
    }
    
    //fprintf pri neuspechu vracia zapornu hodnotu
    if (fprintf(file, "P6\n%u %u\n255\n", p->xsize, p->ysize) < 0) 
    {
        warning_msg("Problem so zapisovanim do suboru %s \n", filename);
        if (fclose(file) == EOF)
			warning_msg("Nepodarilo sa zavriet subor %s \n", filename);	 
        return -1;
    }

    // Zapis dat do suboru s naslednou kontrolou
    if (fwrite(p->data, sizeof(char), bin_size, file) != bin_size) 
    {
        warning_msg("Nepodarilo sa zapisat bin. data suboru %s\n", filename);
        if (fclose(file) == EOF)
    		warning_msg("Nepodarilo sa zavriet subor %s \n", filename);
        return -1;
	}

    if (fclose(file) == EOF)
       warning_msg("Nepodarilo sa zavriet subor %s \n", filename);

    return 0;
}