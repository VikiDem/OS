#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int pAB;
int pCB;

int main(int argc, char *argv[]){
    pAB = atoi(argv[1]);
    pCB = atoi(argv[2]);

    int sizeA;
    int sizeB;
    while(read(pAB, &sizeA, sizeof(int)) > 0 && read(pCB, &sizeB, sizeof(int)) > 0){
        printf("[B programm] A length: %d; C length: %d\n", sizeA, sizeB);
    }

}