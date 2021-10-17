#include <stdio.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

#include "structdefs.h"

/* This function initialises each floor and forks persons of that floor. */
void initFloorsForkPersons(int shmidLifts, int shmidFloors){
    int perm = S_IRWXU | S_IRWXG | S_IRWXO;
    int shmid = shmget(IPC_PRIVATE, NFLOOR*sizeof (FloorInfo), perm | IPC_CREAT);

    if(shmid == -1){
        int errsv = errno;
        printf("Error: shmget | %s | %d \n", __func__ ,errsv);
    }

    FloorInfo *floors = (FloorInfo*)shmat(shmid, NULL, 0);
    if((void*)floors == (void*)-1){
        int errsv = errno;
        printf("Error: shmat | %s | %d \n", __func__ , errsv);
    }

    for(int i=0; i<NFLOOR; i++){
        initFloorForkPersons(i + 1, floors + i, shmidLifts, shmidFloors);
    }

    return;
}

void forkLifts(int shmidLifts, int shmidFloors){
    int perm = S_IRWXU | S_IRWXG | S_IRWXO;
    int shmid = shmget(IPC_PRIVATE, NLIFT*sizeof (LiftInfo), perm|IPC_CREAT);

    if(shmid == -1){
        int errsv = errno;
        printf("Error: shmget | %s | %d \n", __func__ ,errsv);
    }

    LiftInfo *lifts = (LiftInfo*) shmat(shmid, NULL, 0);
    if((void*) lifts == (void*)-1){
        int errsv = errno;
        printf("Error: shmat | %s | %d \n", __func__ , errsv);
    }

    for(int i=0; i<NLIFT; i++){
        forkLift(i + 1, lifts + i, shmidLifts, shmidFloors);
    }
    return;
}

int main() {
    int perm = S_IRWXU | S_IRWXG | S_IRWXO;
    int shmidLifts = shmget(IPC_PRIVATE, NFLOOR*sizeof (FloorInfo), perm | IPC_CREAT);
    if(shmidLifts == -1){
        int errsv = errno;
        printf("[%s] shmid  | %d", "Lifts", errsv);
    }

    int shmidFloors = shmget(IPC_PRIVATE, NLIFT*sizeof (LiftInfo), perm|IPC_CREAT);
    if(shmidFloors == -1){
        int errsv = errno;
        printf("[%s] shmid | %d", "Floors", errsv);
    }

    initFloorsForkPersons(shmidLifts, shmidFloors);
    forkLifts(shmidLifts, shmidFloors);
    printf("Hello, World!\n");
    return 0;
}
