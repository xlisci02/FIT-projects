/***********************************
* File: c016.c
* Autor: Matus Liscinsky
* Login: xlisci02
* Date: 19.11.2017
************************************/

/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, říjen 2014
**                              Radek Hranický, listopad 2015
**                              Radek Hranický, říjen 2016
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for (int i = 0; i < HTSIZE; ++i)
		(*ptrht)[i] = NULL;
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if(!ptrht || !key) // kontrola ukazatelov
		return NULL;
	for (tHTItem * tmp = (*ptrht)[hashCode(key)]; tmp!=NULL; tmp=tmp->ptrnext) // cyklus cez zretazeny zoznam
	{
		if(strcmp(key,tmp->key)==0) // zhodne kluce = najdena polozka tabulky
			return tmp;  // vracia ukazatel na najdenu polozku
	}
	return NULL; // inak NULL
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	if(!ptrht || !key) // kontrola ukazatelov
		return;
	tHTItem * tmp = htSearch(ptrht, key); // ak uz je v tabulke, aktualizuju sa data
	if(tmp){
		tmp->data = data;
		return;
	}
	
	tmp = (tHTItem*) malloc(sizeof(tHTItem)); // ak nieje v tabulke, vytvori sa nova polozka
	if(!tmp)
		return;
	tmp->key = (char *) malloc(sizeof(char)*(strlen(key)+1)); // alloc pre kluc
	if(!tmp->key){
		free(tmp);
		return;
	}
	
	strcpy(tmp->key,key); // kopiruje kluc
	tmp->data=data;  // prepise data 
	tmp->ptrnext=(*ptrht)[hashCode(key)]; // vlozi na zaciatok zoznamu 
	(*ptrht)[hashCode(key)]=tmp;
	
}


/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	tHTItem* tmp = htSearch(ptrht,key); // ak polozku najde vracia data, inak NULL
	return (tmp)? &tmp->data:NULL;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	if(!ptrht || !key) // kontrola ukazatelov
		return;
	tHTItem * prev = NULL; // pomocny ukazatel
	for (tHTItem * tmp = (*ptrht)[hashCode(key)]; tmp!=NULL; tmp=tmp->ptrnext){ // prechod zoznamom
		
		if(strcmp(tmp->key,key)==0){
			if(tmp == (*ptrht)[hashCode(key)])
				(*ptrht)[hashCode(key)]=tmp->ptrnext; // ak to je prvy prvok 
			else
				prev->ptrnext = tmp->ptrnext; // ak nieje prvy
			if(tmp->key)
				free(tmp->key); // uvolnenie pamate kluca
			free(tmp); // uvolnenie pamate polozky
			return;
		}
		prev=tmp;
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	if(!ptrht) // kontrola ukazatela
		return;
	tHTItem * xchg = NULL; // pomocny ukazatel

	for(unsigned i = 0; i < HTSIZE; i++){ // prechod riadkami tabulky
		for(tHTItem * tmp = (*ptrht)[i]; tmp!=NULL; tmp = xchg){ //prechod zoznamom
			xchg=tmp->ptrnext; // uchovanie ukazatela na nasledovnika
			if(tmp->key) 
				free(tmp->key);  // uvolnenie pamate kluca
			free(tmp); // uvolnenie pamate polozky

		}
		(*ptrht)[i]=NULL;  // ako po inicializacii
	}
 		
}
