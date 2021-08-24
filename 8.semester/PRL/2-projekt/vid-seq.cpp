#include <stdio.h>
#include <math.h>
#include <cmath>
#include <iostream>

#define SIZE 8
using namespace std;


// void after_max_prescan(float * angle, float * max_prev_angle, unsigned size){
//     for (unsigned i=1; i < size; i++){
//         max_prev_angle[i] = fmax(angle[i-1], angle[i]);
//     }
// }

void reduce(int n, float * arr){
    for(int j=0; j <= log2(n)-1; j++){
        for(int i=0; i <= n-1; i+=pow(2,j+1)){
            cout<< "zapis do" << i+(int)pow(2,j+1)-1 << " z "<< i+(int)pow(2,j)-1 << endl;
            arr[i+(int)pow(2,j+1)-1] = (arr[i+(int)pow(2,j)-1] + arr[i+(int)pow(2,j+1)-1]);
        }
        cout << "--" <<endl;
    }
}


void pre_reduce(int * sum, int numprocs, int numvals, int * arr){
    for (int i = 0; i < numprocs; i++){ // each procesor
        cout << "zapis z " << (numvals/numprocs)*i << " do " << i << endl;
        sum [i] = arr[(numvals/numprocs)*i];
        for (int j = 1; j < (numvals/numprocs); j++){
            cout << "ku " << i << " priratam " << (numvals/numprocs)*i+j << endl;
            sum[i] = sum [i] + arr[(numvals/numprocs)*i +j];
        }
    }
}


void print_arr(char * name, float * arr, unsigned size){
    cout << name << endl;
    for (unsigned i=0; i < size; i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}


// namiesto float pouzit double pri uhloch
float * down_sweep(int numprocs, float *angles, int angles_arr_size){
    float * max_prev_angle = new float [angles_arr_size];

    for (int i =0; i< angles_arr_size; i++)
        max_prev_angle[i]=angles[i];

    max_prev_angle[SIZE-1] = 0.0;
    print_arr("vo fukncii max prev angle", max_prev_angle, SIZE);
    for (int d=log2(numprocs)-1; d >= 0; d--){
        cout << "d= " << d << endl;
        for (int i=0; i <= numprocs-1 ; i+=pow(2,d+1)){
            cout << "cyklus i= " << i << " a d = "<< d <<   endl;
            // odlozenie hodnoty left child
            float t = max_prev_angle[i+(int)(pow(2,d))-1];
            // left child = parent
            cout << "zapisujem na index " << i+(int)pow(2,d)-1 << endl;
            max_prev_angle[i+(int)pow(2,d)-1] = max_prev_angle[i+(int)pow(2,d+1)-1];
            // right child = parent + left child
            cout << "zapisujem na index " << i+(int)pow(2,d+1)-1 << endl;
            max_prev_angle[i+(int)pow(2,d+1)-1] = t + max_prev_angle[i+(int)pow(2,d+1)-1];
        }
    }
    return max_prev_angle;
}



void print_int_arr(char * name, int * arr, unsigned size){
    cout << name << endl;
    for (unsigned i=0; i < size; i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}
int main(void){

    // int alt [SIZE] = {290,291,291,289,350,378,405,295};
    int alt [SIZE] = {3,1,7,0,4,1,6,3};
    // int numprocs = ceil(pow(2,ceil(log2(SIZE)))/2);
    int numprocs = SIZE;
    float max_prev_angle [SIZE] = {0};
    float angle [SIZE] = {0};
    int result [SIZE] = {0};
    cout << numprocs << endl;
    int sum [numprocs] = {0};
    print_int_arr("alt:", alt , SIZE);

    // prepocet na uhly
    for (int i=1; i < SIZE; i++){
        // angle[i] = atan ((alt[i] - alt[0])/i);
        angle[i] = (float)alt[i];
    }
    angle[0] = alt[0];
    print_arr("angle:", angle, SIZE);


    // PRE-REDUCE, iba pre pochopenie algoritmu
    // pre_reduce(sum, numprocs, SIZE, alt);
    // for (int k= 0; k < numprocs; k++){
    //     cout << sum[k] << " ";
    // }
    // cout << endl;


    reduce(SIZE,angle);

    print_arr("angle after reduce:", angle , SIZE);


    float * max_prev_angle1 = down_sweep(numprocs, angle, SIZE);
    print_arr("after max prev angle", max_prev_angle1,SIZE);

    // after_max_prescan(angle, max_prev_angle1, SIZE);
    // print_arr("after_max_prescan:", max_prev_angle1, SIZE);
    for (int i=1; i < SIZE; i++){
        result[i] = (angle[i]>max_prev_angle1[i])?1:0;
    }
    print_int_arr("result:", result, SIZE);
}