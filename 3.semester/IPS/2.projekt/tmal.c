
/*******************************
 * IPS 2017 - project 2 - Thread Memory Allocator.
 * File: tmal.c
 * Compiled: g++ (Ubuntu/Linaro 6.3.0-18ubuntu2~16.04) 6.3.0 20170519
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 14.11.2017
 * Preklad:
 * gcc -std=c99 -Wall -Wextra tmal.c -c
 * gcc -std=c99 -Wall -Wextra test_tmal.c -c
 * gcc -o test_tmal test_tmal.o tmal.o
 * *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tmal.h"

// Inicializacia globalneho ukazatela na tabulku blokov
struct blk_pool_t *blks_table = NULL;

// alokacia pamate struktury pre kazde vlakno, obsahujuce informacie o heape kazdeho vlakna
struct blk_pool_t *tal_alloc_blks_table(unsigned nthreads){
	if(blks_table != NULL) // tabulka nie je prazdna, vracia NULL
		return NULL;
	blks_table = (struct blk_pool_t *) malloc(sizeof(struct blk_pool_t)*nthreads);
		return blks_table; // pri chybe mallocu sa vracia NULL
}

// "konstruktor" pre strukturu uchovavajuci informacie o bloku
void blk_ctor(struct blk_t *blk)
{
	blk->ptr=NULL; // ukazatel na blok, na zacatku neukazuje nikam
	blk->prev_idx=-1; // index bloku vlavo
	blk->next_idx=-1; // index bloku vpravo
	blk->used=false; // info o pouzivani bloku, na kt. ukazuje
}

// alokuje miesto pre pole struktur obsah. info o blokoch na heape pre konkretne vlakno
struct blk_t *tal_init_blks(unsigned tid, unsigned nblks, size_t theap)
{
	blks_table[tid].blks=(struct blk_t *) malloc(sizeof(struct blk_t)*nblks); 
	if(!blks_table[tid].blks) // kontrola mallocu
		return NULL;
	blks_table[tid].nblks=nblks; // nast. pocet blokov vlakna
	
	for (unsigned i = 0; i < nblks; ++i) // vola sa "konstruktor" pre vsetky struktury metadat blokov
		blk_ctor(&(BLK(tid,i)));

	void * ptr = malloc(theap); // alokuje sa miesto pre cely heap 
	if(!ptr){					// kontrola mallocu
		free(blks_table[tid].blks);  
		blks_table[tid].heap_size=0;
		return NULL;
	}
	BLK(tid, 0).ptr = ptr; // na alok. mieste ukazuje prva metadata struktura v poli 
	BLK(tid, 0).size = theap; // nast. velkosti do metadat
	//blks_table[tid].blks[0].ptr = ptr;
	//blks_table[tid].blks[0].size = theap;
	blks_table[tid].heap_size=theap; // aktualizacia dat o thread heape

	return blks_table[tid].blks; // vracia sa ukazatel na prvu metadata strukturu v poli

}

// rozdelenie dvoch blokov 
int tal_blk_split(unsigned tid, int blk_idx, size_t req_size){
	unsigned i;
	for (i = 0; i < blks_table[tid].nblks; ++i) // hladame v metadatach o blokoch nepouzity ukazatel
	{
		if(BLK(tid, i).ptr == NULL) // nasiel sa 
			break;
	}

	if(i == blks_table[tid].nblks) // nenasiel sa, vracia -1, ako index znamena neex
		return -1;
	
	struct blk_t * right = &(BLK(tid,i));// ukazatel na metadata noveho bloku
	struct blk_t * left = &(BLK(tid,blk_idx));// uk. na metadata rozdelovaneho bloku
	
	*right = *left; // ukazuju na rovnake miesto 
	right->ptr = ((char*) left->ptr) + req_size; // adresova aritmetika, posun ukazatela 
	right->size -= req_size; // nast. do metadat velkost bloku na ktory ukazuje
	right->prev_idx = blk_idx; // nast. index bloku nalavo

	left->size=req_size;  // nast. do metadat novu velkost bloku na ktory ukazuje
	left->next_idx=i; // nast. index bloku napravo
	return i;  	// vracia index praveho bloku
}

// spojenie dvoch blokov do jedneho
void tal_blk_merge(unsigned tid, int left_idx, int right_idx){

	BLK(tid,left_idx).size += BLK(tid,right_idx).size; // spocita velkosti praveho aj laveho bloku
	BLK(tid,left_idx).next_idx = BLK(tid,right_idx).next_idx; // index nasledujuceho bude nasledujuci praveho
	
		if(BLK(tid,right_idx).next_idx >= 0 ) // ak ma pravy blok nasledovnika
		BLK(tid,(BLK(tid,right_idx).next_idx)).prev_idx = left_idx; // jeho predchodca bude prave nas blok
	
	blk_ctor(&(BLK(tid,right_idx)));// pravy blok sa stane nepouzivanym, ako po inicializacii
}

// alokacia pamate 
void *tal_alloc(unsigned tid, size_t size){
	size = (size+(sizeof(size_t)-1))/(sizeof(size_t)) * (sizeof(size_t)); // zarovnani // allignment
	unsigned i;
	int index=0;
	for (i = 0; i < blks_table[tid].nblks; ++i) // hladanie vhodneho bloku
	{
		if(index >=0){
			if(BLK(tid,index).ptr && 
				!(BLK(tid,index).used)  && // not used 
				BLK(tid,index).size >= size //dostatocne velka velkost
				)
				break;
			index=BLK(tid,index).next_idx; // index dalsieho bloku
		}
	}
	
	if(i == blks_table[tid].nblks) // nenasiel sa
		return NULL;

    if(BLK(tid,index).size - size >= sizeof(size_t)) // rozdeli blok, ak to ma zmysel
    	tal_blk_split(tid, index, size);

    BLK(tid,index).used=true; // blok bude pouzivany
	return BLK(tid,index).ptr; // vracia ukazatel na blok
}

// realokacia 
void *tal_realloc(unsigned tid, void *ptr, size_t size){
	size = (size+(sizeof(size_t)-1))/(sizeof(size_t)) * (sizeof(size_t)); // vypocita velkost so zarovnanim / alignment
	unsigned i;
	if(!ptr)
		return tal_alloc(tid,size); // neukazuje na zadne miesto, zavola sa rovno alloc, neni co reallocovat

	for (i = 0; i < blks_table[tid].nblks; ++i) // hlada sa blok,ktory s ktorym sa bude pracovat, predany cez parameter
	{
		if(BLK(tid,i).ptr == ptr)//nasiel sa blok
			break;
	}
	if(i == blks_table[tid].nblks) // nenasiel sa blok
		return NULL;

	struct blk_t *tmp = &(BLK(tid,i)); //ziskanie ukazatela na metadata najdeneho bloku 

	if(size <= tmp->size){ // v pripade potreby mensieho bloku zabola split
		if(BLK(tid,i).size - size >= sizeof(size_t)){ // >= or >
    		tal_blk_split(tid, i, size);	
    		return BLK(tid,i).ptr;// vrati ukazatel na blok
    	}
	}
	if(tmp->next_idx >=0){ // ak ma suseda vpravo
		struct blk_t *next = &BLK(tid,tmp->next_idx); // uk. na nasledujuci blok/ na suseda vpravo
		if(!next->used && tmp->size + next->size >= size) // ak je sused nepouzivany a spolu maju dostatocnu velkost 
			tal_blk_merge(tid, i, tmp->next_idx); // spoja sa tieto 2 bloky do jedneho
		return tmp->ptr;// vrati ukazatel na novy/ staronovy blok
	}
	
	void * new = tal_alloc(tid, size); // v pripade ze nemozno vyuzit nasledujuci, alokujeme novy
	if(!new)
		return NULL;

	memcpy(new, tmp->ptr, tmp->size); // prekopirujeme obsah stareho do noveho bloku
	tmp->used=false; // stary oznacime za nepouzivany
	if(tmp->next_idx >=0 && BLK(tid,tmp->next_idx).used==false) // ak je mozny merge s blokom napravo, vykona sa merge
		tal_blk_merge(tid, i, tmp->next_idx);
	if(tmp->prev_idx >=0 && BLK(tid,tmp->prev_idx).used==false)// ak je mozny merge s blokom nalavo, vykona sa merge
		tal_blk_merge(tid, tmp->prev_idx, i);
	return new; // vrati ukazatel na novy blok


}

// uvolnenie pamate
void tal_free(unsigned tid, void *ptr){
	if(ptr){ // kontrola platneho ukazatela
		unsigned i;	
		for (i = 0; i < blks_table[tid].nblks; ++i)
		{
			if(BLK(tid,i).ptr == ptr) // blok sa nasiel 
				break;
		}
		if(i != blks_table[tid].nblks){ 
			struct blk_t *tmp = &BLK(tid,i); // ukazatel na metadata ruseneho bloku
			tmp->used=false; // oznacime ho za nepouzivany 
			if(tmp->next_idx >=0 && BLK(tid,tmp->next_idx).used==false) // ak je mozny merge s blokom napravo, vykona sa merge
				tal_blk_merge(tid, i, tmp->next_idx);
			if(tmp->prev_idx >=0 && BLK(tid,tmp->prev_idx).used==false) // ak je mozny merge s blokom nalavo, vykona sa merge
				tal_blk_merge(tid, tmp->prev_idx, i);
		}
	}
}
