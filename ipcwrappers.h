//
// Created by hiro on 10/17/21.
//

#ifndef ASSIGNMENT6_IPCWRAPPERS_H
#define ASSIGNMENT6_IPCWRAPPERS_H
#include "structdefs.h"

/* Initialise the pointers to lifts and floor variable. */
int init(int shmidLifts, int shmidFloors, LiftInfo **lifts, FloorInfo** floors);

/* Detach the shared memory from the processes VA space. */
int release(LiftInfo *lifts, FloorInfo *floors);

#endif //ASSIGNMENT6_IPCWRAPPERS_H
