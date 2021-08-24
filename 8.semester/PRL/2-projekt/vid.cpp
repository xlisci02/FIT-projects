/*
* Project: Solution of Line-of-Sight problem using OpenMPI library
* Author: Matus Liscinsky (xlisci02)
* Mail: xlisci02@stud.fit.vutbr.cz
* Date: 19.4.2020
*/

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>


using namespace std;

#define is_last_index(i, numprocs) (i == (numprocs - 1))
#define TAG 0
#define MASTER_ID 0
#define decode(x) ((x)?((x == 1)?'v':'_'):'u')
#define up_sweep_sender(myrank, j, numprocs) ((!is_last_index(myrank, numprocs)) && \
    ((myrank + (int)pow(2, j) + 1) % (int)pow(2, j + 1) == 0))
#define up_sweep_reciever(myrank, j) ((myrank + 1) % (int)pow(2, j + 1) == 0 && \
    myrank != MASTER_ID)
#define down_sweep_comm_node(myrank, d) ((myrank + 1)%d == 0)
#define down_sweep_parent(myrank, d) ((myrank + 1) % (2*d) == 0)

//#define TEST

void get_elapsed_time(double start, double finish, int rank){
    double loc_elapsed = finish - start;
    double elapsed;
    // pouziva sa iba pri zberani casovych udajov
    MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, MASTER_ID, MPI_COMM_WORLD);
    #ifdef TEST
    if (rank == MASTER_ID)
        cout << elapsed * 1000000 << endl;
    #endif
}

void send_int_to(int dst_rank, int *value, int count = 1){
    MPI_Send(&(*value), count, MPI_INT, dst_rank, TAG, MPI_COMM_WORLD);
}

void recv_int_from(int src_rank, int *value, MPI_Status *stat, int count = 1){
    MPI_Recv(&(*value), count, MPI_INT, src_rank, TAG, MPI_COMM_WORLD, &(*stat));
}

void send_double_to(int dst_rank, double *value, int count = 1){
    MPI_Send(&(*value), count, MPI_DOUBLE, dst_rank, TAG, MPI_COMM_WORLD);
}

void recv_double_from(int src_rank, double *value, MPI_Status *stat, int count = 1){
    MPI_Recv(&(*value), count, MPI_DOUBLE, src_rank, TAG, MPI_COMM_WORLD, &(*stat));
}

vector<int> read_input(char * str){
    vector<int> alts;
    stringstream ss(str);
	string s;
    int alt;
    for (int i = 0; getline(ss, s, ','); i++) {
        alt = stoi(s);
        alts.push_back(alt);
    }
    return alts;
}

int get_proc_value_count(int numvals, int numprocs, int rank){
    if ((numvals % numprocs) == 0) return numvals/numprocs;
    int chunk_size = ((float)numvals/numprocs);
    return ((rank < (numvals % numprocs))?chunk_size+1:chunk_size);
}

int get_global_alt_index(int numvals, int numprocs, int rank){
    if ((numvals % numprocs) == 0)
        return rank *  numvals/numprocs;
    int bigger_chunks_before = (numvals % numprocs);
    if (rank < bigger_chunks_before)
        bigger_chunks_before -= (bigger_chunks_before - rank);
    int smaller_chunks_before = rank - bigger_chunks_before;
    int chunk_size = ((float)numvals/numprocs);
    return bigger_chunks_before * (chunk_size + 1) + smaller_chunks_before * chunk_size;
}

void distribute(int *recieved_alts, int numprocs, int numvals){
    int values_count;
    int alt_i = get_proc_value_count(numvals, numprocs, MASTER_ID); // pocet hodnot pre MASTER procesor

    for (int proc_i = 1; proc_i < numprocs; alt_i += values_count, proc_i++)
    {
        values_count = get_proc_value_count(numvals, numprocs, proc_i);
        send_int_to(proc_i, &(recieved_alts[alt_i]), values_count);
    }
}

double compute_angle(int global_alt_index, int observer_alt, int alt){
    if(!global_alt_index){
        return -M_PI_2;
    }
    return atan((double)(alt - observer_alt)/global_alt_index);
}

double seq_reduce(double * recieved_angles, int values_count){
    double max = recieved_angles[0];
    for (int i = 1; i < values_count; i++)
        max = fmax(max, recieved_angles[i]);
    return max;
}

void up_sweep(int myrank, int numprocs, double * myangle, MPI_Status *stat){
    double neighangle;
    for (int j = 0; j <= log2(numprocs)-1; j++)
    {
        if (up_sweep_sender(myrank, j, numprocs))
        {
            send_double_to(myrank + pow(2, j), &(*myangle));
        }
        else if (up_sweep_reciever(myrank, j))
        {
            recv_double_from(myrank - pow(2, j), &neighangle, &(*stat));
            *myangle = fmax(*myangle, neighangle);
        }
    }
}

void down_sweep(int myrank, int numprocs, double * mymax_prev_angle, MPI_Status *stat){
    double tmp;
    for (int k = log2(numprocs)-1; k >= 0; k--){
        int d = pow(2,k);
        if(down_sweep_comm_node(myrank, d)){
            // iniciator komunikacie
            if(down_sweep_parent(myrank, d)){
                send_double_to((myrank-d),&(*mymax_prev_angle));
                recv_double_from((myrank-d), &tmp, &(*stat));
                *mymax_prev_angle = fmax(*mymax_prev_angle, tmp);
            }
            else{
                tmp = *mymax_prev_angle;
                recv_double_from((myrank+d), &(*mymax_prev_angle), &(*stat));
                send_double_to((myrank+d), &tmp);
            }
        }
    }
}


void seq_prescan(double * max_prev_angles, double * recieved_angles, double mymax_prev_angle, int values_count){
    max_prev_angles[0] = mymax_prev_angle;
    double current_max_angle = recieved_angles[0];
    for (int i = 1; i < values_count; i++){
        max_prev_angles[i] = fmax(mymax_prev_angle, current_max_angle);
        current_max_angle = fmax(current_max_angle, recieved_angles[i]);
    }
}

void recieve_results(MPI_Status * stat, vector<int> & results, int numprocs, int numvals){
    for(int proc_i = 1; proc_i < numprocs; proc_i++){
        int values_count = get_proc_value_count(numvals, numprocs, proc_i);
        int * res = new int [values_count];
        recv_int_from(proc_i, &res[0],  &(*stat), values_count);
        for(int j = 0; j < values_count; j++)
            results.push_back(res[j]);
        delete [] res;
    }
}

int main(int argc, char *argv[])
{
    int numprocs;                // pocet procesorov
    int numvals;                 // pocet hodnot
    int myrank;                  // moj rank
    double myangle;              // moja hodnota uhla
    double mymax_prev_angle;     // moja hodnota v ramci max-prescan
    MPI_Status stat;             // status struktura, obsahuje: source, tag, error
    int observer_alt;            // nadm. vyska pozorovatela
    vector<int> all_alts_vec;    // vektor nadmorskych vysok
    int * recieved_alts;         // pole priradenych nadmorskych vysok

    // inicializacia MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // MASTER nacita vsetky nadmorske vysky a ziska hodnotu pozorovatela a celkovy pocet hodnot
    if(myrank == MASTER_ID){
        all_alts_vec = read_input(argv[1]);
        numvals = all_alts_vec.size();
        observer_alt = all_alts_vec[0];
    }

    // rozoslanie nadm. vysky pozorovatela a informacie o pocte hodnot
    MPI_Bcast(&numvals, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);
    MPI_Bcast(&observer_alt, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);

    // pocet prvkov na procesor (ktory procesor dostane kolko hodnot)
    int values_count = get_proc_value_count(numvals, numprocs, myrank);


    // MASTER procesor posiela nadmorske vysky ostatnym procesorom
    if (myrank == MASTER_ID){
        distribute(all_alts_vec.data(), numprocs, numvals);
        recieved_alts = all_alts_vec.data();
    }
    // vsetky procesory (okrem mastra) prijmaju svoje pole hodnot
    else{
        recieved_alts = new int [values_count];
        recv_int_from(MASTER_ID, &recieved_alts[0], &stat, values_count);
    }

    #ifdef TEST
    double start;               // zaciatok merania pre testovanie
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    #endif

    // prepocet na uhly
    double * recieved_angles = new double [values_count]; // prepocitane uhly z nadmorskych vysok `recieved_alts`
    for (int i = 0; i < values_count; i++){
        int global_alt_index = get_global_alt_index(numvals, numprocs, myrank) + i;
        recieved_angles[i] = compute_angle(global_alt_index,  observer_alt, recieved_alts[i]);
    }

    // sekvencny reduce, z `recieved_angles` sa stane jedna hodnota `myangle`
    myangle = seq_reduce(recieved_angles, values_count);

    // UP-SWEEP (reduce)
    up_sweep(myrank, numprocs, &myangle, &stat);

    // CLEAR + kopia hodnoty
    if(is_last_index(myrank, numprocs))
        mymax_prev_angle = -M_PI_2;
    else
        mymax_prev_angle = myangle;

    // DOWN-SWEEP
    down_sweep(myrank, numprocs, &mymax_prev_angle, &stat);

    // sekvencny vypocet max_prev_angle pre ostatne uhly v ramci procesora
    double * max_prev_angles = new double [values_count];
    seq_prescan(max_prev_angles, recieved_angles, mymax_prev_angle, values_count);

    int * results = new int [values_count]; // 'u'(0), 'v'(1), alebo '_' (inak, napr. -1)
    for (int i = 0; i < values_count; i++)
        results[i] = (recieved_angles[i] > max_prev_angles[i]);

    #ifdef TEST
    get_elapsed_time(start, MPI_Wtime(), myrank);
    #endif

    // ZISKAVANIE & VYPIS VYSLEDKOV
    if(myrank == MASTER_ID) {
        vector<int>all_results_vec;

        // miesto pozorovatela
        results[0]=-1;
        // vysledky MASTER procesora
        all_results_vec.insert(all_results_vec.end(), results, results + values_count);

        // ZISKAVANIE VYSLEDKOV od ostatnych procesorov
        recieve_results (&stat, all_results_vec, numprocs, numvals);

        // VYPIS
        #ifndef TEST
        for (int i = 0; i < all_results_vec.size(); i++)
            cout << (decode (all_results_vec[i])) << ((is_last_index(i, numvals))?"\n":",");
        #endif
    }

    // zasielanie vysledkov MASTER procesoru
    else {
        send_int_to(MASTER_ID, &results[0], values_count);
    }

    // uvolnenie pamate
    delete [] results;
    delete [] max_prev_angles;
    delete [] recieved_angles;
    if (myrank != MASTER_ID)
        delete [] recieved_alts;

    MPI_Finalize();
    return 0;
}
