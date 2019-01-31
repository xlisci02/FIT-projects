/**
  tail2.cc
  Riešenie IJC-DU2, príklad a), 23.4.2017
  Autor: Matúš Liščinský, FIT
  Login: xlisci02
  Preložené: g++ 5.4.0 (merlin)
  Program v C++ pre vypis poslednych n riadkov zo suboru viz. POSIX tail.
  Na rozdiel od tail.c dlzka riadku je neobmedzena
**/

#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
#include <queue>

/*
* Funkcia process_arguments sluzi na:
* Spracovanie argumentov
* V pripade uspechu vracia pocet skontrolovanych argumentov +1 (nulty argument)
* Ak nastane chyba vracia -1
*/
int process_arguments(int argc, char * argv[], long * number)
{
	using namespace std;
	if(argc > 4 || argc < 1)
	{
		cerr << "[ERR] Chybny pocet argumentov\n" << endl;
		return -1;
	}
	else if (argc == 1 || argc ==2) 
	{
		return argc;
	}
	
	else if(argc == 3)
	{
		char * ptr = NULL;
		* number = strtol(argv[2],&ptr,10);
		if( strcmp(argv[1],"-n")==0 && *ptr =='\0' && *number >= 0)
			return argc;	
		else 
		{
			cerr << "[ERR] Chybne argumenty [" << argv[1] << " " << argv[2] << "]" << endl;
			return -1;
		}	
	}
	else //(argc == 4)
	{
		
		char * ptr = NULL;
		* number = strtol(argv[2],&ptr,10);
		
		if(strcmp(argv[1],"-n")==0 && *ptr =='\0' && *number >= 0)
			return argc;	
		else 
		{
			cerr << "[ERR] Chybne argumenty [" << argv[1] << " " << argv[2] << " " << argv[3] << "]" <<  endl ;
			return -1;
		}		

	}
	
}

/*
* Funkcia main
*/
int main(int argc, char * argv[])
{
	using namespace std;
	ios::sync_with_stdio(false);
	long number=10;
	ifstream f;
	istream * file = &cin; // defaultne nastaveny standardny vstup
	int ARG = process_arguments(argc,argv,&number);
	
	if(ARG!=argc)
		return 1;
	
	if(ARG%2==0) //ak bol zadany subor, pocet argumentov moze byt 2 alebo 4
	{ 
		f.open(argv[ARG-1]);
		if(f.is_open())
			file=&f;
	}	

	string buff; // buffer pre ukladanie znakov riadku cez funkciu getline
	queue<string> bufArray; // buffer pre ukladanie riadkov

	while (getline(*file,buff)) // citanie riadkov
	{
		bufArray.push(buff); // pushne riadok do bufArray
		if(bufArray.size() > (unsigned) number) // ak je ich uz prilis urobi pop
			bufArray.pop();
	}

	while(bufArray.size()) // Vypis bufArray
	{
		cout << bufArray.front() << endl;
		bufArray.pop(); 
	}
	
	f.close();
}
