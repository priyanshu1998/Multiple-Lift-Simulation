#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "structdefs.h"

int main(int argc, char **argv){
    if (argc != 5){
        printf("'Person' process executed with incorrect no. of parameters\n");
        return 1;
    }

    int src = atoi(argv[1]);
    int des = atoi(argv[2]);
    int shmidLifts = atoi(argv[3]);
    int shmidFloors = atoi(argv[4]);

    Person P = initPerson(src, des);

    printf("$ %s %d %d %d %d\n",argv[0], P.src, P.des, shmidLifts, shmidFloors);
    return 0;
}

