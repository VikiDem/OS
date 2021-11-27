#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int pAC;
int pCA;
int pCB;

int main(int argc, char *argv[]){
    pAC = atoi(argv[1]);
    pCA = atoi(argv[2]);
    pCB = atoi(argv[3]);

    int sizeA;
    while(read(pAC, &sizeA, sizeof(int))> 0){
        char line[sizeA+1];
        line[sizeA] = '\0';
	    read(pAC, line, sizeA * sizeof(char));
        printf("[C Programm] Got from A: %s\n", line);

        int good = 1;
        write(pCA, &good, sizeof(int));

        write(pCB, &sizeA, sizeof(int));

    }

}