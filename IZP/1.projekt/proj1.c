/*
 * @author  Matus Liscinsky
 * @email xlisci02@stud.fit.vutbr.cz
 * @version 1.10, 06/11/16
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
*Funkcia pre pouzitie argumentu -r 
*/
void argumentR()
{
	int c;
	int p=0;
	char array[2];
	int help;

	while ((c=getchar())!=EOF) //nacitavanie znakov so standardneho vstupu
	{	
		if(!(isspace(c))) 	// ignoruje biele znaky
		{	if(p<2)
		  	{
				array[p]=c;  // ukladanie znaku do pola
				p++;
			}		
	 	}
		/*
		* Ak sa pole naplni tak..
		*/
		if(p>=2) 
		{	// overi ci je znak hexadecimalnym znakom
		    if(((array[0]>='0' && array[0]<='9') || ( array[0]>='a' && array[0]<='f') || ( array[0]>='A' && array[0]<='F') )
		 && (( array[1]>='0'&& array[1]<='9') 	|| ( array[1]>='a' && array[1]<='f') ||	( array[1]>='A' && array[1]<='F')) )
		    {
			// pomocou funkcie strtol zapisem do premennej help cislo v z hexadecimalnej do desiatkovej sustavy a vypisem ho ako znak
			help=strtol(array,NULL,16);
			printf("%c",help);
			p=0;  // reset pocitadla pre dalsiu dvojicu znakov
		    } 
 		    
		    else {fprintf(stderr,"\nWrong input data\n");p=0;break;}	// ak znaky niesu hexadecimalne
 
		}	
	}
	/*
	*  Ak je pocet znakov na stupe neparny
	*/
	if (((c=getchar())==EOF) && p==1) 
	{	// posledny nacitany znak posuniem na nizsiu poziciu a na vyssiu dosadim 0
		array[1]=array[0]; 
		array[0]='0';
		// Dalej sa opat vykonavaju kroky na overovanie znakov a ich vypis
		if(((array[0]>='0' && array[0]<='9') || ( array[0]>='a' && array[0]<='f') || ( array[0]>='A' && array[0]<='F') )
		 && (( array[1]>='0'&& array[1]<='9') 	|| ( array[1]>='a' && array[1]<='f') ||	( array[1]>='A' && array[1]<='F')) )
		    {
			help=strtol(array,NULL,16);
			printf("%c",help);
			p=0;
		    } 
 		else {fprintf(stderr,"\nWrong input data\n");p=0;}	
 	}
}

/*
*Funkcia pre pouzitie argumentu -S 
*/
void argumentS(int s) 
{
	int c;	
	int p=0;
	int pole[s+1];
	int priznak=0;
	while((c=getchar())!=EOF) // nacitavanie znakov z stdin
	{	
		if(((isblank(c)) || (isprint(c)))&& (p<s)) // chceme iba prazdne a tlacitelne znaky  
		{	
			pole[p]=c;
			p++;
		}
		else if (p>=s) // ak narazime na zly znak alebo retazec uz je rovny premennej s 
		{	//najprv vypis cele pole
			if(priznak==0)				
			{	for(int i=0; i<p; i++)
					printf("%c",pole[i]);
			}
			priznak++;
			// a vypisuj dalej prazdne a tlacitelne znaky 
			if((isblank(c)) || (isprint(c)))
			{
				printf("%c",c);
			}
			// v pripade chybneho znaku chod na dalsi riadok a resetuj premenne pre nacitavanie dalsieho retazca
			else 
			{	printf("\n");
				priznak=0; p=0;
			}
		}
		else p=0; // ak bude retazec prilis kratky reset pocitadla p	
	}
	
}

/*
*Funkcia pre pouzitie argumentu -x 
*/
void argumentX()
{
	int a;
	while((a=getchar())!=EOF) //nacitavanie znaku z stdin a ich vypis v pozadovanej podobe
	{
		printf("%02x",a);
	}
	printf("\n");
}
/*
* Funkcia pre spustanie bez argumentov
*/
void noArguments()
{ 
	int c;
	int i=0;
	int pos=0;
	int riadok=-1;
	int pole[16];
	while((c=getchar())!=EOF) // nacitavanie znakov z stdin
	{
		if(i%16==0)	// pocitadlo znakov (i) je delitelne 16 -> presun do noveho riadka 
		{
			riadok++;		
			if(riadok!=0)	// na zaciatku bez odsadenia
				printf("\n");
								
			printf("%08x  ",i);	// vypis pozicie prveho bajtu  
			printf("%02x ",c);  // vypis  hexadecimalnej hodnoty bajtu
			pos=i-(riadok*16);  
			pole[pos]=c;		//zapis do pola na spravnu poziciu 
			i++;
		}

		else if ((i+1)%16==0)  //posledny znak v riadku, za nim sa vypisuju nacitane bajty v tlacitelnej podobe
		{
			printf("%02x ",c);	// vypis hexadecimalnej hodnoty bajtu  
			pos=i-(riadok*16);
			pole[pos]=c;		// zapis do pola na spravnu poziciu
			i++;
			printf (" |");

			for (int a=0;a<(pos+1);a++)
			{
				if(isprint(pole[a]))	  // ak to nie je tlacitelny znak, vypise '.'
					printf("%c",pole[a]);
				else printf(".");
			}
			printf("|");
		}
			
		else 	// ak to nema byt prvy ani posledny znak v riadku 
		{
			printf("%02x ",c); 	// vypis hexadecimalnej hodnoty bajtu  
			pos=i-(riadok*16);
			pole[pos]=c;		// zapis do pola na spravnu poziciu
			i++;
			if(pos==7)			// ak sa nachadzame v strede prida este jednu medzeru
				printf(" ");
		}
	}
	 
	// ak sa po skonceni nacitavania nenachadzame na poslednom znaku v riadku 
	if(pos!=15)
	{
		for(int i=0; i<15-pos;i++)  // doplnenie medzier v oblasti kde sa nachadzaju hexadecimalne hodnoty bajtov
			printf("   ");
		if (pos<8 && pos!=7)  
			printf(" ");
		printf(" |");

		for (int a=0;a<(pos+1);a++)  // vypis tlacitelnej podoby nacitanych znakov
		{		
			if(isprint(pole[a]))	 // ak nie je tlacitelne, vypis '.'
				printf("%c",pole[a]); 
			else printf(".");
		}
		
		for(int i=0;i<15-pos;i++)  // doplnennie medzier v oblasti kde sa nachadza tlacitelna podoba bajtov
		{
			printf(" ");
		}
		printf("|");
	}
	printf ("\n");
}
/*
* Fukncia pre pouzitie argumentov [-s M] [-n N] 
*/
void withArguments(int s, int  n)
{ 
	int c;
	int i=0;
	int pos=0;
	int count=0;
	int riadok=-1;
	int pole[16];
	int ibaS =0; 
	while(count!=s)  // preskocenie  prvych M znakov
	{
		c=getchar();
		count++;
		if(c==EOF) // ak je retazec mensi ako M-> narazi na koniec -> ukonci program
		{	
			exit(1); 
		}
	}
  
	if(n==-1) // v pripade pouzitia samotneho -s, bez -n
	{
		ibaS=1;
		n=1;
	}
 
	while((c=getchar())!=EOF && i<n  )
	{
		if(ibaS)n++;  // v pripade ze je pouzite iba -s, pre vypis az dokonca kedze vzdy bude platit ze i<n
		if(i%16==0)		// zaciatok noveho riadku
		{
			riadok++;
			if(riadok!=0)  // na zaciatku bez odsadenia
				printf("\n");
			printf("%08x  ",count+i);  //aktualna pozicia prveho bajtu
			printf("%02x ",c);			// vypis hexadecimalnej hodnoty bajtu  
			pos=i-(riadok*16);
			pole[pos]=c;		//vypis na spravnu poziciu
			i++;
		}

		else if ((i+1)%16==0)  // koniec riadku 
		{
			printf("%02x ",c);
			pos=i-(riadok*16);
			pole[pos]=c;
			i++;
			printf (" |");

			for (int a=0;a<(pos+1);a++)		// vypis textovej podoby
			{
				if(isprint(pole[a]))    // nahradi netlacitelny znak za '.'
					printf("%c",pole[a]);
				else printf(".");
			}
			printf("|");
		}
		// ani zaciatok ani koniec riadku
		else
		{
			printf("%02x ",c);
			pos=i-(riadok*16);
			pole[pos]=c;
			i++;
			if(pos==7)
				printf(" ");
		}
	}
	// ak po skonceni nacitavania znakov niesme na konci nasho riadku
	if(pos!=15)
	{
		for(int i=0; i<15-pos;i++)   // doplnenie medzier v oblasti kde sa nachadzaju hexadecimalne hodnoty bajtov
			printf("   ");
		if (pos<8 && pos!=7)
			printf(" ");
		printf(" |");

		for (int a=0;a<(pos+1);a++)  // vypis tlacitelnej podoby nacitanych znakov
		{		
			if(isprint(pole[a]))	  // ak nie je tlacitelne, vypis '.'
				printf("%c",pole[a]);
			else printf(".");
		}
			
		for(int i=0;i<15-pos;i++)   // doplnennie medzier v oblasti kde sa nachadza tlacitelna podoba bajtov
		{
			printf(" ");
		}
		printf("|");
	}
	printf ("\n"); 
}
/*
* Funkcia pre porovnanie retazcov
*/
int compareStrings(char x[], char y[]) 
{
	int i=0,sign=0;
	/*
	* Ak sa aspon v jednom znaku lisia, vyskoci z cyklu a zmeni hodnotu premennej sign
	*/
	while(x[i]!='\0' && y[i]!='\0')
	{
        if(x[i]!=y[i])		
		{
			sign=1;
			break;
        }
        	i++;
 	}
	
   	if (sign==0 && x[i]=='\0' && y[i]=='\0')  // su zhodne iba v pripade ak plati tato podmienka, funkcia vrati hodnotu 1
        	return 1;
    	else								// inac funkcia vrati 0
         	return 0;
}

int main(int argc, char *argv[])
{
	/*
	*Ak spustime program bez argumentov, zavola sa na to funkcia
	*/
 	if (argc==1)
		noArguments();
	
	/*
	*Ak spustime program s 1 argumentom, zavola sa funkcia ktora k nemu patri
	* Ak to nebude ziaden z parametrov -r a -x program vypise chybu
	*/
	else if (argc==2)
	{	if(compareStrings(argv[1], "-r"))   
			argumentR();
		else if(compareStrings(argv[1], "-x"))
			argumentX();
		else fprintf(stderr,"Try again, wrong argument used.\n");
	}		
	/*
	*Ak spustime program s 2 argumentami
	*/
	else if(argc==3)
	{
		int sum=0;
		int right=1;
	  	
		for(int i=0; argv[2][i]!=0;i++)
			{	if(argv[2][i]<'0'||argv[2][i]>'9') // kontrola ci je 2. argument cislo 
					right=0;break;	
			}
			sum= strtol(argv[2], NULL,10);  // prevod z retazca na cislo
			if(right==1)			
			{
				if(compareStrings(argv[1], "-s"))  // v pripade ze bol 1.argument -s
				{
					if(sum>=0)	// cislo musi byt nezaporne
						withArguments(sum,-1);
					else fprintf(stderr,"Try again, wrong argument used.\n"); 
				}
				else if (compareStrings(argv[1],"-n")) // v pripade ze bol 1.argument -n
				{
					if(sum>0)	// cislo musi byt kladne
						withArguments(0,sum);
					else fprintf(stderr,"Try again, wrong argument used.\n");
				}
				else if (compareStrings(argv[1],"-S")) // v pripade ze bol 1.argument -S
				{
					if(sum<200)  // cislo musi byt mensie ako 200
						argumentS(sum);
					else fprintf(stderr,"Try again, wrong argument used.\n");
				}
				else fprintf(stderr,"Try again, wrong argument used.\n");
			}
			else fprintf(stderr,"Try again, wrong argument used.\n");

	}
	
	/*
	*Ak spustime program so 4 argumentami
	*/
	else if(argc==5)
	{
		int sum[2];
		int right=1;
		
		// ak su to argumenty -s M -n N alebo -n N -s M
		if(((compareStrings(argv[1],"-s")) && (compareStrings(argv[3],"-n"))) || (compareStrings(argv[1],"-n" )&&
					(compareStrings(argv[3],"-s"))))
					
		/*
		* kontrola ci su argumenty M a N cisla
		*/
		{	for(int x=2;x<5;x=x+2)
		 	{	for(int i=0;argv[x][i]!='\0';i++)
				{
					if((argv[x][i]<'0') || (argv[x][i]>'9'))
					{	
						right=0;break;
					}	
				}	
		  	}
			// prevod z retazca na cislo 
			sum[0]= strtol(argv[2], NULL,10);
			sum[1]= strtol(argv[4], NULL,10);
			
			// ak M a N su cisla ..
			if(right==1)
			{	if(compareStrings(argv[1],"-s"))  			// ak je to v poradi -s -n 
				{	if(sum[0]>=0 && sum[1]>0)  				// argument M musi byt nezaporny a argument N kladny
						withArguments(sum[0],sum[1]); 
					else fprintf(stderr,"Try again, wrong argument used.\n");  // ak cislo nevyhovuje podmienke
				}	
				else if (compareStrings(argv[1], "-n"))		  // ak je to v poradi -n -s 
				{	if(sum[1]>=0 && sum[0]>0)				// opat kontrola, ci su cisla v spravnom intervale 
						withArguments(sum[1],sum[0]);  		// v tomto pripade je to iba otocene, podla definicie funkcie withArguments()
					else fprintf(stderr,"Try again, wrong argument used.\n");  
				}
			}	
			else fprintf(stderr,"Try again, wrong argument used.\n"); // v pripade zlych argumentov vypise chybovu hlasku
		}
		else fprintf(stderr,"Try again, wrong argument used.\n");  
	}
	// ak je program spusteny s inym poctom argumentov
	else fprintf(stderr,"Try again, wrong argument used.\n");
	return 0;
}