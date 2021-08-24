/*
* Project: Odd-even transposition sort using OpenMPI library
* Author: Matus Liscinsky (xlisci02)
* Mail: xlisci02@stud.fit.vutbr.cz
* Date: 27.3.2020
*/

#include <mpi.h>
#include <iostream>
#include <fstream>

using namespace std;

#define left_neigh(x) (x-1)
#define right_neigh(x) (x+1)
#define is_odd(x) (x%2)
#define is_last_index(i, numprocs) (i == (numprocs-1))
#define TAG 0
#define MASTER_ID 0

// #define TEST1
// #define TEST2

void get_elapsed_time(double start, double finish, int rank){
    double loc_elapsed = finish-start;
    double elapsed;
    MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, MASTER_ID, MPI_COMM_WORLD);
    if(rank == MASTER_ID) cout << elapsed * 1000000 << endl;
}

void send_to(int dst_rank, unsigned char * value){
    MPI_Send(&(*value), 1, MPI_BYTE, dst_rank, TAG, MPI_COMM_WORLD);
}

void recv_from(int src_rank, unsigned char * value, MPI_Status * stat){
    MPI_Recv(&(*value), 1, MPI_BYTE, src_rank, TAG, MPI_COMM_WORLD, &(*stat));
}

unsigned char * read_input(const char input_file [], int numprocs, unsigned char * myval){
    FILE * pFile = fopen (input_file , "rb" );
    unsigned char * numbers = new unsigned char [numprocs];
    fread(numbers, sizeof(unsigned char), numprocs, pFile);
    fclose (pFile);
    *myval = numbers[MASTER_ID];
    return numbers;
}

void distribute(unsigned char * numbers, int numprocs){
    for (int i = 0; i < numprocs; i++){
        #if !defined(TEST1) && !defined(TEST2)
        cout << unsigned(numbers[i]) << ((is_last_index(i, numprocs))?"\n":" ");
        #endif
        if (i == MASTER_ID) continue;
        send_to(i, &(numbers[i]));
    }
}


void recieve_results(MPI_Status * stat, int numprocs, unsigned char * results){
    unsigned char neighval;
    for(int i = 0; i < numprocs; i++){
        if (i == MASTER_ID) continue;
        recv_from(i, &neighval, &(*stat));
        results[i] = neighval;
    }
}


void compare_and_swap(int myrank, unsigned char * myval, unsigned char * neighval){
    if(*neighval > *myval){
        send_to(left_neigh(myrank), &(*myval));
        *myval = *neighval;
    }
    else send_to(left_neigh(myrank), &(*neighval));
}


int main(int argc, char *argv[])
{
    int numprocs;               // pocet procesorov
    int myrank;                 // moj rank
    unsigned char myval;        // moja hodnota cisla
    unsigned char neighval;     // hodnota suseda
    MPI_Status stat;            // status struktura, obsahuje: source, tag, error
    const char input_file [] = "numbers";

    //inicializacia MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    //MASTER procesor nacita subor cisel
    unsigned char * numbers;
    if(myrank == MASTER_ID) numbers = read_input(input_file, numprocs, &myval);

    #ifdef TEST1
    double start1;               // zaciatok merania pre 1.test
    MPI_Barrier(MPI_COMM_WORLD);
    start1 = MPI_Wtime();
    #endif

    //MASTER procesor po jednom cisle posle ostatnym procesorom
    if(myrank == MASTER_ID) distribute(numbers, numprocs);

    //vsetky procesory (okrem mastra) prijmaju svoju hodnotu
    else recv_from(MASTER_ID, &myval, &stat);

    //limity
    int last_odd = 2*(numprocs/2)-1;
    int last_even = 2*((numprocs-1)/2);
    int loop_cycles = (numprocs/2.0)+0.6; // zaokruhlenie nahor, potrebne pri neparnych cislach

    #ifdef TEST2
    double start2;               // zaciatok merania pre 2.test
    MPI_Barrier(MPI_COMM_WORLD);
    start2 = MPI_Wtime();
    #endif

    for(int i=1; i <= loop_cycles; i++){
        // neparne (liche) procesory posielaju svoje cislo a cakaju na nizsie
        if(is_odd(myrank) && (myrank < last_even)){
            send_to(right_neigh(myrank), &myval);
            recv_from(right_neigh(myrank), &myval, &stat);
        }
        // parne (sude) procesory prijmaju od licheho cislo a vracaju mensie v porovnani so svojim
        else if(myrank <= last_even && myrank != MASTER_ID){
            recv_from(left_neigh(myrank), &neighval, &stat);
            compare_and_swap(myrank, &myval, &neighval);
        }
        else{}

        // parne (sude) procesory posielaju svoje cislo a cakaju na nizsie
        if((!is_odd(myrank) || myrank == MASTER_ID) && (myrank < last_odd)){
            send_to(right_neigh(myrank),&myval);
            recv_from(right_neigh(myrank), &myval, &stat);
        }
        // neparne (liche) procesory prijmaju od sudeho cislo a vracaju mensie v porovnani so svojim
        else if(myrank <= last_odd){
            recv_from(left_neigh(myrank), &neighval, &stat);
            compare_and_swap(myrank, &myval, &neighval);
        }
        else{}
    }

    #ifdef TEST2
    get_elapsed_time(start2, MPI_Wtime(), myrank);
    #endif

    //ziskavanie vysledkov
    unsigned char * results;
    if(myrank == MASTER_ID) {
        results = new unsigned char [numprocs];
        recieve_results (&stat, numprocs, results);
    }
    else send_to(MASTER_ID, &myval);

    #ifdef TEST1
    get_elapsed_time(start1, MPI_Wtime(), myrank);
    #endif

    //vypis vysledkov a uvolnenie pamate
    if(myrank == MASTER_ID){
        results[MASTER_ID] = myval;
        #if !defined(TEST1) && !defined(TEST2)
        for(int i = 0; i < numprocs; i++){
            cout << unsigned(results[i]) << endl;
        }
        #endif
        delete [] results;
        delete [] numbers;
    }
    MPI_Finalize();
    return 0;
}
