#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

#include "structdefs.h"
#include "ipcwrappers.h"

int main(int argc, char **argv){
    if(argc != 4){
        printf("'Lift' Process executed with incorrect no. of parameters\n");
        return 1;
    }

    int liftno = atoi(argv[1]);
    int shmidLifts = atoi(argv[2]);
    int shmidFloors = atoi(argv[3]);

    LiftInfo *lifts = NULL;
    FloorInfo *floors = NULL;

    init(shmidLifts, shmidFloors, &lifts, &floors);

    LiftInfo L = lifts[liftno-1]; //as liftno is 1 indexed

    printf("# %s %d %d %d\n", argv[0], liftno, shmidLifts, shmidFloors);
    /*
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        P(L.stops)
    }
    #pragma clang diagnostic pop
     */

    release(lifts, floors);
    return 0;
}

