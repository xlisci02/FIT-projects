/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
	int id;
	float x;
	float y;
};

struct cluster_t {
	int size;
	int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
   	assert(c != NULL);
   	assert(cap >= 0);
   	// TODO
    
	c->obj=malloc(sizeof(struct obj_t) *cap);
    c->capacity=cap;
	if(c->obj==NULL)
		c->capacity=0;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
    free(c->obj);
   	c->size=0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
   	 // TUTO FUNKCI NEMENTE
 	assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
    	return c;

    size_t size = sizeof(struct obj_t) * new_cap;

   	void *arr = realloc(c->obj, size);
   	if (arr == NULL)
       	return NULL;

   	c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	// TODO
  	/*
		Ak je dostatocna kapacita prida objekt na koniec shluku
		ak nie rozsiri ho a ak sa resize podaril tak prida objekt
		na konec shluku
	*/ 
	if((c->capacity)!=(c->size))
    {
		c->obj[c->size]=obj;
		c->size++;
    }			
    else 
    {	
		c=resize_cluster(c,(c->capacity)+CLUSTER_CHUNK);
    	if(c!=NULL)
		{	
			c->obj[c->size]=obj;
			c->size++;
		}
    }		
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
   	assert(c1 != NULL);
   	assert(c2 != NULL);
		
	// TODO
	
	for(int i=0; i<c2->size;i++)
		append_cluster(c1,c2->obj[i]);
	sort_cluster(c1);	
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
	assert(idx < narr);
	assert(narr > 0);
	// TODO
	
	free(carr[idx].obj);
	for(int i=idx; i<narr-1;i++)
	{
		carr[i]=carr[i+1];	
	}
	return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1 != NULL);
 	assert(o2 != NULL);
 	// TODO

    return sqrtf((o2->x-o1->x)*(o2->x-o1->x)+(o2->y-o1->y)*(o2->y-o1->y));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
  	assert(c1->size > 0);
    assert(c2 != NULL);
	assert(c2->size > 0);
	// TODO

	float max=0;
	float tmp=0;
	for(int i=0; i<c1->size;i++)
	{
		for(int j=0; j<c2->size;j++)
		{
			tmp=(obj_distance(&c1->obj[i],&c2->obj[j]));
			if(tmp>max)
				max=tmp;
		}
	}
	return max;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
	// TODO
	
	float min = INFINITY;
	float tmp;
	for(int i=0; i<narr;i++)
	{
		for(int j=i+1;j<narr;j++)
		{	
			tmp=cluster_distance(&carr[i],&carr[j]);
			if(tmp<min)
			{
				min=tmp;
				*c1=i;	
				*c2=j;
			}
		}
	}
}					

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{/* Funkcia pre kontrolu argumentov
 * Vyuzivaju sa ukazovatele pre priamu zmenu hodnoty
 * Parameter *x -> retazec ktory sa kontroluje
 * Parameter a-> ci ide o double alebo int
 * Parameter *flag -> urcuje ci vsetko prebehlo v poriadku
 * Parameter *res1 -> zapise sa hodnota ako double
 * Parameter *res2 -> zapise sa hodnota ako int
 * */
    	// TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
	const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
   	qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    	// TUTO FUNKCI NEMENTE
	for (int i = 0; i < c->size; i++)
    {
		if (i) putchar(' ');
       	printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Vlastna funkcia pre uvolnenie pamate
*/
void clear_all(struct cluster_t *c, int narr)
{	
	for(int i=0;i<narr;i++)
		clear_cluster(&c[i]);
	free(c);
}	

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
  	assert(arr != NULL);
    // TODO
	int i=0,count;
	char line_end;
	int err=0;
	float a,b,c;
	struct cluster_t *array;
	
	//Nacitanie suboru
	FILE *f =fopen(filename, "r");
	
	// Ak sa subor nepodari nacitat vrati 0, ktora reprezentuje chybu  
	if(f==NULL)
	{
		*arr=NULL;
		fprintf(stderr,"[ERR][FILE]=File does not exist.\n");
		return 0;
	}

	//citanie 1.riadku
	fscanf(f,"count=%d",&count);
	
	// V pripade zleho 1.riadku alebo neplatneho cisla vrati 0
	if(count<=0)
	{	
		*arr=NULL;
		fprintf(stderr,"[ERR][FILE]=Check the input file(line 1)\n");
		fclose(f);
		return 0;
	}
	
	// Pole ktore bude obsahovat id objektov
	float id_arr[count];
	
	// Alokacia pamate pre pole clustrov
	array=malloc((sizeof(struct cluster_t))*count);
	
	// V pripade chyby vrati 0
	if(array==NULL)
	{	
		*arr=NULL;
		clear_all(array,0);
		fprintf(stderr,"[ERR][MEMORY]=Memory alloc failure.\n");
		fclose(f);
		return 0;
	}
		
	// Nacitavanie objektov zo suboru
	while(fscanf(f,"%f%f%f",&a,&b,&c)==3)
    {
		// 	Overenie udajov (ci su to cele cisla)
		if(a!=(int)a || b!=(int) b || c!=(int) c)
			break;
		
		// 	Kontrola, ze 1 riadok = 1 objekt
		while((line_end=fgetc(f))!='\n')
		{
			if(line_end==EOF)
			{
				break;
			}
			else if(!isblank(line_end)) 
			{
				err=1;
				break;
			}
		}	
		
		// 	Overovanie jedinecnosti id objektov
		id_arr[i]=a;
		for(int j=0;j<i;j++)
		{	
			if(id_arr[j]==id_arr[i])
			{	
				err=1;
				break;
			}
		}
	
		// 	Kontrola suradnic, a id objektu 
		if(a>INT_MAX || b<0 || b>1000 || c<0 || c>1000 || err)
		{
			break;
		}
		
		//	Inicializacia clustra a priradenie hodnot
		init_cluster(&array[i],1);
		
		// 	V pripade chyby alokacie vrati 0
		if(array[i].capacity==0)
		{
			clear_all(array,i);
			*arr=NULL;
			fprintf(stderr,"[ERR][MEMORY]=Memory alloc failure.\n");
			fclose(f);
			return 0;	
		}
		array[i].obj->id=a;
		array[i].obj->x=b;
		array[i].obj->y=c;		
		array[i].size=1;
		i++;
		
		// 	Nacita iba prvych [count] clustrov, ostatne odignoruje
		if(i==count)
			break;
	}
	
	/* 
		Ak dojde k chybe pri citani (zly format vstupnych dat, duplikovane id objektov...)
		vypise chybu, na ktorom riadku v subore je chyba a vrati 0
	*/
	if(i!=count)
	{			
		clear_all(array,i);
		*arr=NULL;
		// Pre spravny vypis ak je chybny pocet clustrov v 1.riadku v subore 
		if(i<count) 
			i=-1;
		fprintf(stderr,"[ERR][FILE]=Check the input file(line %d)\n",i+2);
		fclose(f);
		return 0 ;
	}
		
	/* 
	   	Ak vsetko je OK zatvori subor, arr ukazuje na prvy prvok a vrati 
	   	pocet nacitanych clustrov
	*/
	fclose(f);
	*arr=&array[0];
	return i;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
   	printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
    	printf("cluster %d: ", i);
    	print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
   	//TODO
	int m,n;
	int result=0;
	char* ptr;
	// Pri chybajucom 2.argumente ho doplni ako 1
	if(argc==2)
	{
		argc++;
		argv[2]="1";
	}
	
	if(argc==3)
	{	
		struct cluster_t *clusters;
		
		// 	Cielovy pocet clustrov, ktore chceme vytvorit
		int clusters_count=strtoul(argv[2],&ptr,10);
		
		// 	V pripade zleho argumentu
		if(*ptr!='\0' || clusters_count<=0)
		{
			fprintf(stderr,"[ERR][ARG]=Number of clusters has to be integer >= 1\n");
			return 1;
		}
	
		// 	f_clusters=(file clusters)=pocet nacitanych clustrov zo suboru
		int f_clusters=load_clusters(argv[1],&clusters);
        
		/* 	Ak vo funkcii load_clusters nastala chyba, ukoncime program
			vratenim 1, chybove hlasenie uz bolo vypisane pomocou funkcie
		*/ 	
		if(f_clusters==0)
		{	
			return 1;
		}
		
		// 	Ak sa rovnaju pocty, iba ich vypise
		if(clusters_count==f_clusters)
			print_clusters(clusters,f_clusters);
		
		/*
			Shlukovanie
			Zmensujeme pocet clustrov najdenim najblizsich clustrov a 
			ich naslednym spojenim kym sa pocet clustrov nerovna 
			pozadovanemu poctu(clusters_count)
		*/
		else if(clusters_count<f_clusters)
		{
			while(f_clusters!=clusters_count)
			{		
				find_neighbours(clusters,f_clusters,&m,&n);
				merge_clusters(&clusters[m],&clusters[n]);
				result=remove_cluster(clusters,f_clusters,n);
				f_clusters=result;
			}
			print_clusters(clusters,result);
		}
		
		/*
		 	Ak je pocet pozadovanych clustrov vacsi nez pocet samotnych objektov
			vypise maximalny pocet ktory moze uzivatel zadat ako argument
			a uvolni pamat
		*/
		else if(clusters_count>f_clusters)
		{
			fprintf(stderr,"[ERR][ARG]=Maximal number of clusters is %d\n",f_clusters);
			clear_all(clusters,f_clusters);
			return 1;
		}
		
		// Na zaver uvolnenie pamate 
		clear_all(clusters,f_clusters);
		return 0;
	}
	
	else
	{
		fprintf(stderr,"[ERR][ARG]=Wrong or no arguments used.\n");
		return 1;		
	}
}
