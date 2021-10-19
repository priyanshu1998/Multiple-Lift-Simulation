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

void swap(int *a, int *b){
    int t = *a;
    *a = *b;
    *b = t;
}

//choices are random
void setChoicePriority(int *choices){
    srand(getpid());
    for(int i=0; i<NFLOOR; i++){choices[i] = i;}

    int endpos = NFLOOR - 1;
    for(int i=0; i<NFLOOR; i++){
        int p = rand() % (endpos+1);
        swap(&(choices[p]), &(choices[endpos])); //pick an index from [0...endpos] and place it at the end of the array.
        endpos--;
    }
}

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
//    printf("$ %s %d %d %d %d\n",argv[0], p.src, p.des, shmidLifts, shmidFloors);

    int choices[NFLOOR];
    setChoicePriority(choices);

    LiftInfo *L = NULL;
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        if(p.des > p.src){
            int src_idx = p.src-1;
            int des_idx = p.des-1;

            //Person waiting to get into the lift--------------
            FloorInfo* X = &(floors[src_idx]);
            V(X->upArrow);
            V(X->arithmetic);
            for(int i=0; i < NLIFT; i++){
                L = &(lifts[i]);
                if(L->position == p.src){
                    X->waitingToGoUp--;
                    L->stops[des_idx]++;
                    break;
                }
            }
            P(X->arithmetic);
            P(X->upArrow);
            //Person has completed getting into the lift--------


            //Person waiting to get out of the lift--------
            V(L->stopsem[des_idx]);
            L->stops[des_idx]--;
            P(L->stopsem[des_idx]);
            //Person has got down from the lift------------

        }
        //================================================


        else{
            //Person wants to go down=========================
            int src_idx = p.src-1;
            int des_idx = p.des-1;

            FloorInfo* X = &(floors[src_idx]);
            V(X->downArrow);
            V(X->arithmetic);
            for(int i=0; i < NLIFT; i++){
                L = &(lifts[i]);
                if(L->position == p.src){
                    X->waitingToGoDown--;
                    L->stops[des_idx]++;
                    break;
                }
            }
            P(X->arithmetic);
            P(X->downArrow);
            //Person has completed getting into the lift--------

            //Person waiting to get out of the lift--------
            V(L->stopsem[des_idx]);
            L->stops[des_idx]--;
            P(L->stopsem[des_idx]);
            //Person has got down from the lift------------
        }
        //================================================

        sleep(1);
        //Person scheduling a new journey-------------------------------
        p.src = p.des;
        while (p.des != p.src){
            p.des = rand()%NFLOOR ;
        }


        int src_idx = p.src-1;
        FloorInfo *X = &(floors[src_idx]);

        V(X->arithmetic);
        if(p.des > p.src){ X->waitingToGoUp++;}
        else /* (p.des < p.src) */{ X->waitingToGoDown++;}
        P(X->arithmetic);
        //Person starts waiting at a floor to get into the lift----------

    }
    #pragma clang diagnostic pop


    release(lifts, floors);
    return 0;
}

