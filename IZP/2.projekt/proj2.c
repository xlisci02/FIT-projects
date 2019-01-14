/*
 * Project #2
 * @author  Matus Liscinsky
 * @email xlisci02@stud.fit.vutbr.cz
 * @version 1.1, 27/11/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


/* Funkcia pre vypocet prirodzeneho logaritmu cisla pomocou Taylorovho polynomu 
 * Podmienky na zaciatku sluzia na to aby sa fukncia spravala ako fukcia log() z math.h
 * medzna hodnota pri rozhodovani pouzitia vzorca je 1
 * na zaciatku sa vypocita hodnota s ktorou chceme pocitat aby sme dosiahli pozadovany vysledok
 * kedze v jednom pripade sa odpocitava v druhom pripocitava, je zvoleny jeden cyklus 
 * v ktorom sa iba pripocitava a ak islo o vzorec s odpocitavanim na konci sa otoci znamienko
 * co je  matematicky korektne 
 * Parameter x znaci cislo z ktoreho chceme vypocitat logaritmus, parameter n pocet opakovani(iteracii)
 * */
double taylor_log(double x, unsigned int n)
{
	int minusFlag=0;
	double s=0.0;
	double base;
	if(x<0)
		return NAN;
	else if(x==0)
		return -INFINITY;
	else if (x==INFINITY)
		return INFINITY;
	else if(x<1)
	{
		minusFlag=1;
		x=1-x;	
	}
	else if(x>=1)
	{
		x=(x-1)/x ;
	}	
	base=x;			
	for(unsigned int i=2;i<=n;i++)
	{
		x=(x*base)*(i-1)/i;	
		s+=x;
	}
	if(minusFlag)
		return -(s+base);
	else return s+base;
}

/* Funkcia pre vypocet prirodzeneho logaritmu cisla pomocou zretazenych zlomkov
 * Opat podmienky na zaciatku aby fukncia pracovala ako funkcia log() z math.h
 * Pocita sa odspodu a premenne sa kazdym opakovanim menia
 * Parameter x znaci cislo v logaritme, parameter n pocet opakovani(iteracii) 
 * */
double cfrac_log(double x, unsigned int n)
{
	if(x<0)
		return NAN;
	else if(x==0)
		return -INFINITY;
	else if(x==INFINITY)
		return INFINITY;
	else 
	{	
		double z=(x-1)/(1+x);
		double cf=1.0;
		double a,b;
		b=(n*2)+3;
		while (n!=0)
		{
			a=n*n*z*z;
			b=b-2;
			cf=a/(b-cf);	
			n--;
		}
		return 2*z/(1-cf);
	}

}
/* Funkcia pre zistenie ci je cislo liche cele cislo
 * je potrebna pri fuknciach taylor_pow a taylorcf_pow
 * Parameter a-> ak je cele liche vrati 1, ak nie vrati 0
 * */
int isOddInt(double a)
{	
	if(a<0)
		a=a*-1;
	if ((long int)a==a && (long int)a%2==!0)
		return 1;
	else 
		return 0;

}

/* Funkcia pre vypocet exponencialnej funkcie pomocou Taylorovho polynomu
 * Na zaciatku su podmienky preto aby sa fukncia chovala ako fukncia pow() z math.h
 * Prirodzeny logaritmus sa pocita pomocou funkcie taylor_log s rovnakym poctom opakovani 
 * Funkcia rata aj s tym, ze logaritmus cisla z intervalu (0,1) je zaporne cislo a zaporne 
 * cislo umocnene na neparne(liche) cislo je opat zaporne, v tychto pripadoch sa vyuziva 
 * jednoducha matematika a to,ze cislo umocnene na zaporny exponent sa rovna 
 * jeho prevratenej hodnote s kladnym exponentom
 * */

double taylor_pow(double x, double y, unsigned int n)
{
	if(x==1 || y==0)
		return 1;
	if(isnan(x) || isnan(y))
		return NAN;
	if((x==-0 || x==0) && y>0 && isOddInt(y))
	{	if(x==0)
			return 0;
		else 
			return -0.0;
	}
	if(x==0 && y>0 &&  !(isOddInt(y)))
		return 0;
	if(x==-1 && isinf(y))
		return 1;
	if(fabs(x)<1 && y==-INFINITY)
		return INFINITY;
	if(fabs(x)>1 && y==-INFINITY)
		return 0;
	if(fabs(x)<1 && y==INFINITY)
		return 0;
	if(fabs(x)>1 && y==INFINITY)
		return INFINITY;
	if(x==-INFINITY && y<0 && isOddInt(y))
		return -0.0;
	if(x==-INFINITY && y<0 && !(isOddInt(y)))
		return 0;
	if(x==-INFINITY && y>0 && isOddInt(y))
		return -INFINITY;
	if(x==-INFINITY && y>0 && !(isOddInt(y)))
		return INFINITY;
	if(x==INFINITY && y<0)
		return 0;
	if(x==INFINITY && y>0)
		return INFINITY;
	if((x==-0 || x==0) && y<0 && isOddInt(y))
	{	if(x==0)
			return INFINITY;
		else 
			return -INFINITY;
	}
	if((x==-0 || x==0) && y<0 && !(isOddInt(y)))
		return INFINITY;

	double s=0.0;
	int sign=0;

	/*(ak je exponent zaporny a cislo nieje z intervalu (0,1) alebo naopak)
	pripocitavalo by sa zaporne cislo, t.j odpocitavalo by sa, otocenim znamienka
	jedneho z clenov sucinu sa ta napravi*/

	if((y<0 && !(x<1 && x>0)) || (!(y<0) && (x<1 && x>0)) )
	{
		sign=1;
		y=y*-1;
	}
	y=y*taylor_log(x,n);
	double base=y;	
	s+=y;	
	for(unsigned int i=2;i<=n;i++)
	{
		y=y*(base/i);
		s+=y;
		
	}
	if(sign)
		return 1/(s+1);
	else 
		return s+1;

}
	
/* Funkcia pre vypocet exponencialnej funkcie pomocou Taylorovho polynomu
 * Od predoslej funckie sa lisi iba v tom ze pre vypocet logaritmu sa
 * pouziva ina funkcia a to cfrac_log
 * */
double taylorcf_pow(double x, double y, unsigned int n)
{
	if(x==1 || y==0)
		return 1;
	if(isnan(x) || isnan(y))
		return NAN;
	if((x==-0 || x==0) && y>0 && isOddInt(y))
	{	if(x==0)
			return 0;
		else 
			return -0.0;
	}
	if(x==0 && y>0 &&  !(isOddInt(y)))
		return 0;
	if(x==-1 && isinf(y))
		return 1;
	if(fabs(x)<1 && y==-INFINITY)
		return INFINITY;
	if(fabs(x)>1 && y==-INFINITY)
		return 0;
	if(fabs(x)<1 && y==INFINITY)
		return 0;
	if(fabs(x)>1 && y==INFINITY)
		return INFINITY;
	if(x==-INFINITY && y<0 && isOddInt(y))
		return -0.0;
	if(x==-INFINITY && y<0 && !(isOddInt(y)))
		return 0;
	if(x==-INFINITY && y>0 && isOddInt(y))
		return -INFINITY;
	if(x==-INFINITY && y>0 && !(isOddInt(y)))
		return INFINITY;
	if(x==INFINITY && y<0)
		return 0;
	if(x==INFINITY && y>0)
		return INFINITY;
	if((x==-0 || x==0) && y<0 && isOddInt(y))
	{	if(x==0)
			return INFINITY;
		else 
			return -INFINITY;
	}
	if((x==-0 || x==0) && y<0 && !(isOddInt(y)))
		return INFINITY;

	double s=0.0;
	int sign=0;
	if((y<0 && !(x<1 && x>0)) || (!(y<0) && (x<1 && x>0)) )
	{
		sign=1;
		y=y*-1;
	}
	y=y*cfrac_log(x,n);
	double base=y;	
	s+=y;	
	for(unsigned int i=2;i<=n;i++)
	{
		y=y*(base/i);
		s+=y;
		
	}
	if(sign)
		return 1/(s+1);
	else 
		return s+1;

}

/* Funkcia pre kontrolu argumentov
 * Vyuzivaju sa ukazovatele pre priamu zmenu hodnoty
 * Parameter *x -> retazec ktory sa kontroluje
 * Parameter a-> ci ide o double alebo int
 * Parameter *flag -> urcuje ci vsetko prebehlo v poriadku
 * Parameter *res1 -> zapise sa hodnota ako double
 * Parameter *res2 -> zapise sa hodnota ako int
 * */
void checkArguments(char *x, char a,int *flag, double *res1,int *res2 )
{
	char *ptr;
	if(a=='D')
	{
		*res1=strtod(x,&ptr);
		if(*ptr=='\0')
			*flag=1;
		else
			*flag=0;
	}
	
	if(a=='I')
	{
		*res2=strtoul(x,&ptr,10);
		if(*ptr=='\0')
			*flag= 1;
		else
			*flag=0;
	}
}
	
/* Funkcia pre vypis 
 * Parameter a -> urcuje ci ide o vypis logaritmu alebo exp.funckie
 * Parametre x,y,y su jednotlive argumenty posielane do funkcii
 */
void myPrint(double x, double y,int z,char a)
{
	if(a=='L')
	{	
		printf("       log(%g) = %.12g\n",x,log(x));
		printf(" cfrac_log(%g) = %.12g\n",x,cfrac_log(x,(unsigned int)z));
		printf("taylor_log(%g) = %.12g\n",x,taylor_log(x,(unsigned int)z));
	}
	if(a=='P')
	{	
		printf("         pow(%g,%g) = %.12g\n",x,y,pow(x,y));	
		printf("  taylor_pow(%g,%g) = %.12g\n",x,y,taylor_pow(x,y,(unsigned int)z));	
		printf("taylorcf_pow(%g,%g) = %.12g\n",x,y,taylorcf_pow(x,y,(unsigned int)z));	
	}

}

int main(int argc, char *argv[])
{
	int flag1=0,flag2=0;
	double number;
	int iter;
	char *err="Try again, wrong argument used.\n";
	
	if(argc==4 && strcmp(argv[1],"--log")==0)
	{
		//kontrola argumentov	
		checkArguments(argv[2],'D',&flag1,&number,&iter);
		checkArguments(argv[3],'I',&flag2,&number,&iter);
		
		// vsetko OK-> vypis, ak nie vypis chybu
		if(flag1 && flag2)
			myPrint(number,number,iter,'L');	
		else 
		{	fprintf(stderr,err);
			return 1;
		}
	}
	
	else if(argc==5 && strcmp(argv[1],"--pow")==0)
	{
		double exp ;
		int flag3=0;
		//kontrola argumentov
		checkArguments(argv[2],'D',&flag1,&number,&iter);
		checkArguments(argv[3],'D',&flag2,&exp,&iter);
		checkArguments(argv[4],'I',&flag3,&exp,&iter);
		
		//ak je to v poriadku -> vypis, ak nie vypis chybu
		if(flag1 && flag2 && flag3)
			myPrint(number,exp,iter,'P');
		else
		{	fprintf(stderr,err);
			return 1;
		}
	}	
	else 
	{	
		fprintf(stderr,err); 
		return 1;
	}
	return 0;
}
