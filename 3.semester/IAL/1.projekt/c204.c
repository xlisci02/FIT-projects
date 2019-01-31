/************************************
 * IAL 2017 - 1.domáca úloha
 * Subor: c204.c
 * Autor: Matus Liscinsky
 * Login: xlisci02
 * Mail: xlisci02@stud.fit.vutbr.cz
 * Datum: 27.10.2017
 * **********************************/

/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, říjen 2017                                        */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;


/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {
	char c;
	if(!stackEmpty(s)){
		stackTop (s,&c); // ziskaj vrchol zasobnika, ak nieje prazdny 
	}
	while(c!='(' && !stackEmpty(s)) { // kym nenarazis na '(' alebo nevyprazdnis zasobnik opakuj cyklus
		postExpr[(*postLen)++] = c; // pridaj vrchol zas. do vyst. pola postExpr
		stackPop(s);				// urob pop zasobnika
		stackTop (s,&c);			// ziskaj novy vrchol zasobnika
	}
	
	stackPop(s); // odstran '(' zo zas., pri prazdnom zasobniku sa nic nedeje

}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {
	char top_c;
	int i=1;
	
	while(i){
		if(stackEmpty(s)){ // urob push ak je zas. prazdny
			stackPush(s,c);
			break;
		}

		stackTop (s,&top_c);		
		if(top_c == '(') { // urob push ak je na vrchole zas. lava zatvorka
			stackPush(s,c);
			break;
		}
		// zanoreny switch -- hra s operatormi
		switch(top_c){
			case '+': 
			case '-': switch(c){
						case '+': 
						case '-': 	stackPop(s);	// rovnaka priorita operatorov 
									postExpr[(*postLen)++]=top_c;
									break;
						case '*': 					// na vrchole je nizsia priorita operatora
						case '/': stackPush(s,c); i=0; break;
					 }break;
			default:	stackPop(s); // rovnaka priorita operatorov || na vrchole je vyssia priorita operatora 
						postExpr[(*postLen)++]=top_c;
						break;
		}
	}	
	
}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {
	char * postExpr = (char *) malloc (sizeof(char)*MAX_LEN); // alokacia miesta pre vyst.pole
	if(!postExpr) // chyba mallocu
		return NULL;
	char c;
	unsigned i=0, pos=0;
	tStack * s = (tStack*) malloc(sizeof(tStack)); // alokacia miesta pre stack
	if(!s){ // chyba mallocu
		free(postExpr);
		return NULL;
	}
	stackInit(s); // inicializacia stacku
	
	while((c=(infExpr[i++]))!='\0'){ // citanie znak po znaku az po koniec vst.retazca
		// identifikatory a cislice 
		if( (c>='a' && c<='z') || 
			(c>='A' && c<='Z') ||
			(c>='0' && c<='9') )
			postExpr[pos++]=c;
		else if(c=='(') // v pripade lavej zatvorky, ju pushni na zas.
			stackPush(s,c);
		else if(c=='+' || c=='-' || c=='*' || c=='/') // ak je znak operator vola sa funkcia doOperation
			doOperation(s, c, postExpr, &pos);
		else if(c==')') // v pripade pravej zatvorky vola funkciu untilLeftPar
			untilLeftPar(s, postExpr, &pos); 
		// koniec vyrazu, uvolni zasobnik, znaky zo zas. vloz do vyst.pola
		else if(c=='='){
			while(!stackEmpty(s))
			{
				stackTop(s,&c);
				postExpr[pos++]=c;
				stackPop(s);
			}
			postExpr[pos++]='='; // na koniec pridaj '=', aby bol vyraz korektne zapisany
		}
	}
	postExpr[pos++]='\0'; // ukoncovaci znak retazca v C 
	free(s);	// uvolni pamat pre stack
	return postExpr; // vrat vyst. retazec
 }

/* Konec c204.c */
