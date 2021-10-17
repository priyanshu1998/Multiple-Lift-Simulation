#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>

#include "structdefs.h"

/* Initialise the pointers to lifts and floor variable. */
static int init(int shmidLifts, int shmidFloors, LiftInfo **lifts, FloorInfo** floors);

/* Detach the shared memory from the processes VA space. */
static int release(LiftInfo *lifts, FloorInfo *floors);

static int init(int shmidLifts, int shmidFloors, LiftInfo **lifts, FloorInfo** floors){
    *lifts = (LiftInfo*)shmat(shmidLifts, 0, 0);
    if((void*)lifts == (void*)-1){
        int errsv = errno;
        printf("[%s] shmat | %d \n", "Lifts" , errsv);
        release(*lifts, *floors);
        exit(EXIT_FAILURE);
    }

    *floors = (FloorInfo*)shmat(shmidFloors, 0, 0);
    if((void*)lifts == (void*)-1){
        int errsv = errno;
        printf("[%s] shmat | %d \n", "Floors", errsv);
        release(*lifts, *floors);
        exit(EXIT_FAILURE);
    }

    return 0;
}


static int release(LiftInfo *lifts, FloorInfo *floors){
    if(lifts != NULL){
        int dtLifts = shmdt(lifts);
        if(dtLifts == -1){
            int errsv = errno;
            printf("[%s] shmdt | %d \n", "Lifts" , errsv);
        }
    }

    if(floors != NULL){
        int dtFloors = shmdt(floors);
        if(dtFloors == -1){
            int errsv = errno;
            printf("[%s] shmdt | %d \n", "Floors" , errsv);
        }
    }

    return 0;
}

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

    assert(lifts != NULL);// theoretically boolean expression should never Fail
    // because for a failed condition the process would have terminated at init stage.

    LiftInfo L = lifts[liftno-1]; //as liftno is 1 indexed


    printf("# %s %d %d %d\n", argv[0], liftno, shmidLifts, shmidFloors);

    release(lifts, floors);
    return 0;
}

