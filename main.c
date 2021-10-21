#include <stdio.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

#include "structdefs.h"
#include "semun.h"
#include "ipcwrappers.h"

/* This function initialises each floor and forks persons of that floor. */
void initFloorsForkPersons(int shmidLifts, int shmidFloors){
    FloorInfo *floors = (FloorInfo*)shmat(shmidFloors, NULL, 0);
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
    LiftInfo *lifts = (LiftInfo*) shmat(shmidLifts, NULL, 0);
    if((void*) lifts == (void*)-1){
        int errsv = errno;
        printf("Error: shmat | %s | %d \n", __func__ , errsv);
    }

    for(int i=0; i<NLIFT; i++){
        forkLift(i + 1, lifts + i, shmidLifts, shmidFloors);
        printf("@ no. %d | floor: %d\n", lifts[i].no, lifts[i].position );
    }
    return;
}


int main() {
    int perm = 0777;//S_IRWXU | S_IRWXG | S_IRWXO;
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

    initLocks(shmidLifts, shmidFloors);
    initFloorsForkPersons(shmidLifts, shmidFloors);
    forkLifts(shmidLifts, shmidFloors);
    while(1);
    return 0;
}
