#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ipcwrappers.h"
#include "semun.h"

int init(int shmidLifts, int shmidFloors, LiftInfo **lifts, FloorInfo** floors){
    *lifts = (LiftInfo*)shmat(shmidLifts, NULL, 0);

    if((void*)(*lifts) == (void*)-1){
        int errsv = errno;
        printf("[%s] shmat | %d \n", "Lifts" , errsv);
        release(*lifts, *floors);
        exit(EXIT_FAILURE);
    }

    *floors = (FloorInfo*)shmat(shmidFloors, NULL, 0);
    if((void*)(*floors) == (void*)-1){
        int errsv = errno;
        printf("[%s] shmat | %d \n", "Floors", errsv);
        release(*lifts, *floors);
        exit(EXIT_FAILURE);
    }

    return 0;
}


int release(LiftInfo *lifts, FloorInfo *floors){
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


void initLocks(int shmidLifts, int shmidFloors){
    // Deviating from implementation of sample code(of slides)

    // technically semctl accepts lvalue as a parameter(as seen in sample code)
    // but as far as I know an unnamed temporary variable is created
    // during the call. So an instance of that variable is created and used
    // I don't have much idea about how internally unions are handled.

    // but I am guessing if union is not defined for a system. program will not compile.
    // hence as per documentation using the defined way of using semctl for SETVAL command.

    int perm = 0777;;//S_IRWXU | S_IRWXO | S_IRWXO;

    LiftInfo *lifts = NULL;
    FloorInfo *floors = NULL;

    init(shmidLifts, shmidFloors, &lifts, &floors);

    union semun zeroint;
    zeroint.val = 0;

    union semun oneint;
    oneint.val = 1;


    for(int i=0; i<NLIFT; i++) {
        LiftInfo *L = &(lifts[i]);
        for(int j=0; j<NFLOOR; j++){
            L->stopsem[j] = semget(IPC_PRIVATE, 1, perm | IPC_CREAT);
            semctl(L->stopsem[j], 0, SETVAL, zeroint);
        }
    }

    for(int i=0; i<NFLOOR; i++) {
        FloorInfo *F = &(floors[i]);
        F->upArrow = semget(IPC_PRIVATE, 1, perm | IPC_CREAT);
        semctl(F->upArrow, 0, SETVAL, zeroint);

        F->downArrow = semget(IPC_PRIVATE, 1, perm | IPC_CREAT);
        semctl(F->downArrow, 0, SETVAL, zeroint);

        F->arithmetic = semget(IPC_PRIVATE, 1, perm | IPC_CREAT);
        semctl(F->arithmetic, 0, SETVAL, oneint);
    }
}

