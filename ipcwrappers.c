//
// Created by hiro on 10/17/21.
//
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ipcwrappers.h"


int init(int shmidLifts, int shmidFloors, LiftInfo **lifts, FloorInfo** floors){
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

