#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "structdefs.h"

int main(int argc, char **argv){
    if(argc != 3){
        printf("'Lift' Process executed with incorrect no. of parameters\n");
        return 1;
    }

    int shmidLifts = atoi(argv[1]);
    int shmidFloors = atoi(argv[2]);


    printf("# %s %d %d", argv[0], shmidLifts, shmidFloors);
    return 0;
}

