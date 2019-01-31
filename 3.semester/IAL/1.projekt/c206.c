	
/************************************
 * IAL 2017 - 1.domáca úloha
 * Subor: c206.c
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 27.10.2017
 * **********************************/

/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->First=NULL;      // zoznam je na zaciatku prazdny                
    L->Act=NULL;		// zoznam je prazdny -> nema aktivny prvok                     
    L->Last=NULL;   	// zoznam je prazdny-> nema ani posledny prvok

}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	tDLElemPtr tmp;
	if(L->First!=NULL) // test na prazdny zoznam
	{
		for(tmp=L->First->rptr; tmp!=NULL;tmp=tmp->rptr) // nastavime sa na 2.prvok, uvolnujeme vzdy lptr
				free(tmp->lptr);	//postupne rusime polozky zoznamu
		free(L->Last);          // uvolnime posledny prvok, ak ma zoznam jeden prvok, je to zaroven posledny
	}
	
	// nastavenie ako po inicializacii - zoznam je prazdny
	L->First=NULL;                      
    L->Act=NULL;             
    L->Last=NULL;  
	
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr new_elem = (tDLElemPtr)malloc(sizeof(struct tDLElem)); // alokacia miesta pre novy prvok
	if(!new_elem){ // chyba mallocu
		DLError();
		return;
	}
	new_elem->data=val; // nast. hodnoty pre novy prvok
	new_elem->lptr=NULL; // prvy prvok zoznamu ma lptr=NULL
	
	// zoznam uz obsahuje prvky, vlozime na prve miesto novy prvok
	if(L->First){
		L->First->lptr=new_elem; 
		new_elem->rptr=L->First;
		L->First=new_elem;
	}
	// zoznam bol prazdny, pridavame prvy prvok do zoz. 
	else{
		new_elem->rptr=NULL; // jediny prvok zoznamu ma aj rptr=NULL
		L->First=L->Last=new_elem; // je zaroven prvy aj posledny
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr new_elem = ( tDLElemPtr )malloc(sizeof(struct tDLElem)); // alokacia miesta pre novy prvok
	if(!new_elem){ // chyba mallocu
		DLError();
		return;
	}
	new_elem->data=val; // nast. hodnoty pre novy prvok
	new_elem->rptr=NULL; // posledny prvok zoznamu ma rptr=NULL
	
	// zoznam uz obsahuje prvky, vlozime na posledne miesto novy prvok
	if(L->Last){
		L->Last->rptr=new_elem;
		new_elem->lptr=L->Last;
		L->Last=new_elem;
	}
	// zoznam bol prazdny, pridavame prvy prvok do zoz. 
	else{
		new_elem->lptr=NULL; // jediny prvok zoznamu ma aj lptr=NULL
		L->First=L->Last=new_elem; // je zaroven prvy aj posledny 
	}
	
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act=L->First; // aktivnym prvkom sa stava prvy prvok
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act=L->Last; // aktivnym prvkom sa stava posledny prvok
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	// ak zoznam je prazdny, vyhod chybu a skonci
	if(L->First==NULL){ 
		DLError();
		return;
	}
	*val=L->First->data; // zoznam NIEJE prazdny, vrat hodnotu prveho prvku 
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	// ak zoznam je prazdny, vyhod chybu a skonci
	if(L->Last==NULL){
		DLError();
		return;
	}
	*val=L->Last->data; // zoznam NIEJE prazdny, vrat hodnotu posledneho prvku 
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First!=NULL){ // kontrola ci je zoznam prazdny
		tDLElemPtr tmp=L->First;
		if(L->First == L->Act) // ak bol prvy aktivny, aktivita sa straca
			L->Act=NULL;
		if(L->First->rptr!=NULL){ // ak to nieje jediny prvok zoznamu
			L->First=L->First->rptr; // prvy prvok bude nasledujuci
			L->First->lptr=NULL;  // pre prvy prvok plati lptr=NULL
		}
		else
			L->First=L->Last=NULL; // ak to bol jediny prvok zoznamu 
		free(tmp); // uvolnenie pamate prveho prvku
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if(L->Last!=NULL){ // kontrola ci je zoznam prazdny
		tDLElemPtr tmp=L->Last;
		if(L->Last == L->Act) // ak bol prvy aktivny, aktivita sa straca
			L->Act=NULL;
		if(L->Last->lptr!=NULL){ // ak to nieje jediny prvok zoznamu
			L->Last=L->Last->lptr; // posledny prvok bude predchadzajuci
			L->Last->rptr=NULL; //pre prvy prvok plati rptr=NULL
		}
		else
			L->First=L->Last=NULL; // ak to bol jediny prvok zoznamu 
		free(tmp); // uvolnenie pamate posledneho prvku
	}
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if(L->Act !=NULL && L->Act != L->Last){ // kontrola aktivity, kontrola ci je aktivny prvok zaroven posledny
		tDLElemPtr tmp=L->Act->rptr;  
		if(tmp==L->Last){  // pripad, kedy mazany prvok je zaroven posledny
			L->Last=L->Act; // posledny prvok bude aktivnym
			L->Last->rptr=NULL; // posledny prvok ma rptr=NULL
		}
		else{ // ak mazany prvok nieje posledny, prenastavime ukazatele 
			L->Act->rptr=tmp->rptr;
			tmp->rptr->lptr=L->Act;
		}
		free(tmp); // uvolnenie pamata mazaneho prvku
	}
			
}


void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if(L->Act !=NULL && L->Act != L->First){ // kontrola aktivity, kontrola ci je aktivny prvok zaroven prvy
		tDLElemPtr tmp=L->Act->lptr;
		if(tmp==L->First){ // pripad, kedy mazany prvok je zaroven prvy
			L->First=L->Act; // prvy prvok bude aktivnym
			L->First->lptr=NULL; // prvy prvok ma rptr=NULL
		}
		else{ // ak mazany prvok nieje prvy, prenastavime ukazatele 
			L->Act->lptr=tmp->lptr;
			tmp->lptr->rptr=L->Act;
		}
		free(tmp); // uvolnenie pamata mazaneho prvku
	}		
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L->Act){ // kontrola aktivity
		tDLElemPtr new_elem = ( tDLElemPtr )malloc(sizeof(struct tDLElem)); // alokacia miesta pre novy prvok
		if(!new_elem){ // chyba mallocu
			DLError();
			return;
		}
		if(L->Act == L->Last){ // ak je aktivny prvok zaroven posledny
			L->Act->rptr=new_elem; // vlozime za akt. prvok novy prvok
			new_elem->lptr=L->Act; // nastavime lptr noveho prvku na aktivny prvok
			new_elem->rptr=NULL; // vlozeny prvok je zaroven poslednym, rptr=NULL
			L->Last=new_elem; // zoznam ma novy ukazatel na posledny prvok
		}
		else{ // ak aktivny prvok NIEJE zaroven posledny, vlozime ho medzi 2 prvky, prenastavime ukazatele 
			L->Act->rptr->lptr=new_elem; 
			new_elem->rptr=L->Act->rptr;
			new_elem->lptr=L->Act;
			L->Act->rptr=new_elem;
		}
		new_elem->data=val; // nastavime hodnotu noveho prvku
	}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L->Act){ // kontrola aktivity
		tDLElemPtr new_elem = ( tDLElemPtr )malloc(sizeof(struct tDLElem)); // alokacia miesta pre novy prvok
		if(!new_elem){ // chyba mallocu
			DLError();
			return;
		}
		if(L->Act == L->First){ // ak je aktivny prvok zaroven prvy
			L->Act->lptr=new_elem; // vlozime pred akt. prvok novy prvok
			new_elem->rptr=L->Act; // nastavime rptr noveho prvku na aktivny prvok
			new_elem->lptr=NULL; // vlozeny prvok je zaroven prvym, lptr=NULL
			L->First=new_elem;  // zoznam ma novy ukazatel na prvy prvok
		}
		
		else { // ak aktivny prvok NIEJE zaroven prvy, vlozime ho medzi 2 prvky, prenastavime ukazatele 
			new_elem->lptr=L->Act->lptr; 
			L->Act->lptr->rptr=new_elem;
			new_elem->rptr=L->Act;
			L->Act->lptr=new_elem;
		}
		new_elem->data=val; // nastavime hodnotu noveho prvku
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(L->Act==NULL){ // kontrola aktivity
		DLError();  // ak zoznam nema akt. prvok, hlasime chybu a koncime
		return;
	}
	*val=L->Act->data;	// inak vracia hodnotu aktivneho prvku
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act!=NULL) // kontrola aktivity 
		L->Act->data=val; // zmena hodnoty aktivneho prvku
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act!=NULL) // kontrola aktivity
		L->Act=L->Act->rptr; // posun aktivity na nasledujuci prvok
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	
	if(L->Act!=NULL) // kontrola aktivity
		L->Act=L->Act->lptr; // posun aktivity na predchadzajuci prvok
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return (L->Act!=NULL); // ak je zoznam aktivny, vyraz je pravdivy, vracia 1, inak vracia 0
}

/* Konec c206.c*/
