#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

#include "structdefs.h"

int main(int argc, char **argv){
    if(argc != 4){
        printf("'Lift' Process executed with incorrect no. of parameters\n");
        return 1;
    }

    int liftno = atoi(argv[1]);
    int shmidLifts = atoi(argv[2]);
    int shmidFloors = atoi(argv[3]);

    LiftInfo *lifts = (LiftInfo*)shmat(shmidLifts, 0, 0);
    if((void*)lifts == (void*)-1){
        int errsv = errno;
        printf("[%s] shmat | %s | %d \n", "Lifts" , errsv);
    }


    LiftInfo L = lifts[liftno-1]; //as liftno is 1 indexed

    FloorInfo *floors = (FloorInfo*)shmat(shmidFloors, 0, 0);
    if((void*)lifts == (void*)-1){
        int errsv = errno;
        printf("[%s] shmat | %d \n", "Floors", errsv);
    }

    printf("# %s %d %d %d\n", argv[0], liftno, shmidLifts, shmidFloors);


    int dtLifts = shmdt(lifts);
    if(dtLifts == -1){
        int errsv = errno;
        printf("[%s] shmdt | %d \n", "Lifts" , errsv);
    }

    int dtFloors = shmdt(floors);
    if(dtFloors == -1){
        int errsv = errno;
        printf("[%s] shmdt | %d \n", "Floors" , errsv);
    }
    return 0;
}

