//
// Created by hiro on 10/17/21.
//

#ifndef ASSIGNMENT6_IPCWRAPPERS_H
#define ASSIGNMENT6_IPCWRAPPERS_H
#include "structdefs.h"

#define P(s) semop(s, &pop, 1) /* pop is the structure we pass for doing the P(s) operation */
#define V(s) semop(s, &vop, 1) /* vop is the structure we pass for doing the V(s) operation */

static struct sembuf pop = {
        .sem_num = 0,
        .sem_flg = SEM_UNDO,
        .sem_op = -1
};


static struct sembuf vop = {
        .sem_num = 0,
        .sem_flg = SEM_UNDO,
        .sem_op = +1
};

/* Initialise the pointers to lifts and floor variable. */
int init(int shmidLifts, int shmidFloors, LiftInfo **lifts, FloorInfo** floors);

/* Detach the shared memory from the processes VA space. */
int release(LiftInfo *lifts, FloorInfo *floors);

void initLocks(int shmidLifts, int shmidFloors);
#endif //ASSIGNMENT6_IPCWRAPPERS_H
