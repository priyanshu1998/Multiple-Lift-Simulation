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
void initFloorsForkPersons(int shmidLifts, int shmidFloors,int instance_cnt_shmid,int exit_check_lock_semid){
    FloorInfo *floors = (FloorInfo*)shmat(shmidFloors, NULL, 0);
    if((void*)floors == (void*)-1){
        int errsv = errno;
        printf("Error: shmat | %s | %d \n", __func__ , errsv);
    }

    for(int i=0; i<NFLOOR; i++){
        initFloorForkPersons(i + 1, floors + i, shmidLifts, shmidFloors, instance_cnt_shmid, exit_check_lock_semid);
    }

    return;
}

void forkLifts(int shmidLifts, int shmidFloors,int instance_cnt_shmid,int exit_check_lock_semid){
    LiftInfo *lifts = (LiftInfo*) shmat(shmidLifts, NULL, 0);
    if((void*) lifts == (void*)-1){
        int errsv = errno;
        printf("Error: shmat | %s | %d \n", __func__ , errsv);
    }

    for(int i=0; i<NLIFT; i++){
        forkLift(i + 1, lifts + i, shmidLifts, shmidFloors, instance_cnt_shmid, exit_check_lock_semid);
    }
    return;
}


int main() {
//   The simulation will run for "about a minute"(30-45 seconds on an avg) before termination
//    and by that time all the processes would have terminated.
//   Dont press CTRL+C as the all the child processes have termination condition coded in them.
//
//   NOTE: The simulation will pause (sleep for 1 sec) when any lift moves to the next floor or changes direction

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

    int instance_cnt_shmid = shmget(IPC_PRIVATE, sizeof (int), perm | IPC_CREAT);
    int *instance_cnt = (int*)shmat(instance_cnt_shmid, NULL, 0);
    *instance_cnt = NFLOOR*MAXPERSON + NLIFT;

    int exit_check_lock_semid = semget(IPC_PRIVATE, 1, perm|IPC_CREAT);
    semctl(exit_check_lock_semid,0, SETVAL, 1);


    initLocks(shmidLifts, shmidFloors);
    forkLifts(shmidLifts, shmidFloors, instance_cnt_shmid, exit_check_lock_semid);
    initFloorsForkPersons(shmidLifts, shmidFloors, instance_cnt_shmid, exit_check_lock_semid);



    while(1){
        P(exit_check_lock_semid);
        if((*instance_cnt) == 0)break;
        V(exit_check_lock_semid);
        sleep(1);
    }

    if(shmdt(instance_cnt) == -1){
        int errsv = errno;
        printf("[%s] shmdt | %s | %d \n", "instance_cnt", "MAIN" , errsv);
    }
    sleep(1);
    rmIPCobject(shmidLifts, shmidFloors, instance_cnt_shmid, exit_check_lock_semid);

    return 0;
}
