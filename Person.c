#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>

#include "structdefs.h"
#include "ipcwrappers.h"

int main(int argc, char **argv){
    if (argc != 5){
        printf("'Person' process executed with incorrect no. of parameters\n");
        return 1;
    }

    int src = atoi(argv[1]);
    int des = atoi(argv[2]);
    int shmidLifts = atoi(argv[3]);
    int shmidFloors = atoi(argv[4]);

    LiftInfo *lifts = NULL;
    FloorInfo *floors = NULL;
    init(shmidLifts, shmidFloors, &lifts, &floors);

    Person p = initPerson(src, des);
    printf("$ %s %d %d %d %d\n",argv[0], p.src, p.des, shmidLifts, shmidFloors);
    /*
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        if(des > src){
            // When the person want to go up.
            V(floors[p.src].upArrow);
            V(floors[p.des].)

            V(floors[p.src].upArrow);
        }
        else{
            P(floors[p.src].downArrow);

            V(floors[p.src].downArrow);
        }
    }
    #pragma clang diagnostic pop
    */

    release(lifts, floors);
    return 0;
}

