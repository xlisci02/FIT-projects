#include <stdio.h>
#include <stdlib.h>

int main(void){
    FILE *fp_in, *fp_out;
    char j;
    fp_in = fopen("xlisci02.txt", "rb");
    fscanf(fp_in, "%c", &j);
    fp_out = fopen("xlisci022.txt", "wb");
    fputc((int)j, fp_out);
    fclose(fp_in);
    fclose(fp_out);
}