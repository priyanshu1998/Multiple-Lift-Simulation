#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include "structdefs.h"
#include "ipcwrappers.h"

char lifttab[2][10] = {"", "\t\t\t\t\t\t\t"};
char personTab[20] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t";


void swap(int *a, int *b){
    int t = *a;
    *a = *b;
    *b = t;
}

int main(int argc, char **argv){
    if (argc != 8){
        printf("'Person' process executed with incorrect no. of parameters\n");
        return 1;
    }
    char msg[100];
    // src des log from initFloorForkPersons;
    int src = atoi(argv[1]);
    int des = atoi(argv[2]);
    int shmidLifts = atoi(argv[3]);
    int shmidFloors = atoi(argv[4]);
    char *name = argv[5];
    int instance_cnt_shmid = atoi(argv[6]);
    int exit_check_lock_semid = atoi(argv[7]);

    int *instance_cnt = shmat(instance_cnt_shmid, NULL, 0);

    LiftInfo *lifts = NULL;
    FloorInfo *floors = NULL;
    init(shmidLifts, shmidFloors,  &lifts, &floors);

//    printf("$ %d %d %d %d %d\n",getpid(), src, des, shmidLifts, shmidFloors);
//    printf("%s#[%s] | %d -> %d\n",personTab, name, src, des);
//    sleep(1);

    LiftInfo *L = NULL;
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        printf("%s#[%s] | %d -> %d\n",personTab, name, src, des);

        if(des > src){
            int src_idx = src-1;
            int des_idx = des-1;
//            printf("%s#[%s] | %d -> %d\n",personTab, name, src_idx, des_idx);

            //Person waiting to get into the lift--------------
            FloorInfo* X = &(floors[src_idx]);
            P(X->upArrow);
            P(X->arithmetic);
            for(int i=0; i<NLIFT; i++){
                L = &(lifts[i]);
                if(L->position == src){
                    L->step_cnt++;
                    sprintf(msg,"%s[%d | Lift_%d] %s person got up at %d\n",lifttab[L->no-1], L->step_cnt, L->no, name, L->position);
                    printf("%s", msg);

                    X->waitingToGoUp--;
                    L->stops[des_idx]++;
                    break;
                }
            }
//            sleep(1);
            V(X->arithmetic);
            V(X->upArrow);
            //Person has completed getting into the lift--------

            sleep(1);

            //Person waiting to get out of the lift--------
            P(L->stopsem[des_idx]);
            L->step_cnt++;
            sprintf(msg,"%s[%d | Lift_%d] %s person got down at %d\n",lifttab[L->no-1],L->step_cnt,L->no, name, L->position);
            printf("%s", msg);
            L->stops[des_idx]--;
            V(L->stopsem[des_idx]);
            //Person has got down from the lift------------

        }
        //================================================


        else{
            //Person wants to go down=========================
            int src_idx = src-1;
            int des_idx = des-1;
//            printf("%s#[%s] | %d -> %d\n",personTab, name, src_idx, des_idx);


            FloorInfo* X = &(floors[src_idx]);
            P(X->downArrow);
            P(X->arithmetic);
            for(int i=0; i<NLIFT; i++){
                L = &(lifts[i]);
                if(L->position == src){
                    L->step_cnt++;
                    sprintf(msg,"%s[%d | Lift_%d] %s person got up at %d\n",lifttab[L->no-1], L->step_cnt, L->no, name, L->position);
                    printf("%s", msg);
                    X->waitingToGoDown--;
                    L->stops[des_idx]++;
                    break;
                }
            }
            V(X->arithmetic);
            V(X->downArrow);
            //Person has completed getting into the lift--------

            sleep(1);
            //Person waiting to get out of the lift--------
            P(L->stopsem[des_idx]);
            L->step_cnt++;
            sprintf(msg,"%s[%d | Lift_%d] %s person got down at %d\n",lifttab[L->no-1],L->step_cnt,L->no, name, L->position);
            printf("%s", msg);

            L->stops[des_idx]--;
            V(L->stopsem[des_idx]);
            //Person has got down from the lift------------
        }
        //================================================
        if(L->step_cnt > TOT_STEPS)
            break;

        sleep(1);
        //Person scheduling a new journey-------------------------------
        src = des;
        while (des == src){
            des = rand()%NFLOOR + 1;
        }


        int src_idx = src-1;
        FloorInfo *X = &(floors[src_idx]);

        P(X->arithmetic);
        if(des > src){ X->waitingToGoUp++;}
        else /* (des < src) */{ X->waitingToGoDown++;}
        V(X->arithmetic);
        //Person starts waiting at a floor to get into the lift----------
    }
    #pragma clang diagnostic pop

    P(exit_check_lock_semid);
    (*instance_cnt)--;
    V(exit_check_lock_semid);

    release(lifts, floors);
    return 0;
}

