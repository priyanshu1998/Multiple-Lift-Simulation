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

    Person p = initPerson(src, des, name);
//    printf("$ %d %d %d %d %d\n",getpid(), p.src, p.des, shmidLifts, shmidFloors);
    printf("%s#[%s] | %d -> %d\n",personTab, name, p.src, p.des);
    sleep(1);
    int choices[NFLOOR];
    setChoicePriority(choices);

    for(int i=0; i<NLIFT; i++){
        close(lifts[i].pipefd[0]);
    }

    LiftInfo *L = NULL;
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        if(p.des > p.src){
            int src_idx = p.src-1;
            int des_idx = p.des-1;

            //Person waiting to get into the lift--------------
            FloorInfo* X = &(floors[src_idx]);
            P(X->upArrow);
            P(X->arithmetic);
            for(int i=0; i<NLIFT; i++){
                L = &(lifts[i]);
                if(L->position == p.src){
                    L->step_cnt++;
                    sprintf(msg,"%s[%d | Lift_%d] %s person got up.\n",lifttab[L->no-1],L->step_cnt, L->no, p.name);
                    printf("%s", msg);
//                    write(L->pipefd[1],msg, strlen(msg));

                    X->waitingToGoUp--;
                    L->stops[des_idx]++;
                    break;
                }
            }
//            sleep(1);
            V(X->arithmetic);
            V(X->upArrow);
            //Person has completed getting into the lift--------


            //Person waiting to get out of the lift--------
            P(L->stopsem[des_idx]);
            L->step_cnt++;
            sprintf(msg,"%s[%d | Lift_%d] %s person got down .\n",lifttab[L->no-1],L->step_cnt,L->no, p.name);
            printf("%s", msg);
//            write(L->pipefd[1], msg, strlen(msg));
            L->stops[des_idx]--;
            V(L->stopsem[des_idx]);
            //Person has got down from the lift------------

        }
        //================================================


        else{
            //Person wants to go down=========================
            int src_idx = p.src-1;
            int des_idx = p.des-1;

            FloorInfo* X = &(floors[src_idx]);
            P(X->downArrow);
            P(X->arithmetic);
            for(int i=0; i<NLIFT; i++){
                L = &(lifts[i]);
                if(L->position == p.src){
                    L->step_cnt++;
                    sprintf(msg,"%s[%d | Lift_%d] %s person got up.\n",lifttab[L->no-1], L->step_cnt, L->no, p.name);
                    printf("%s", msg);
//                    write(L->pipefd[1], msg, strlen(msg));
                    X->waitingToGoDown--;
                    L->stops[des_idx]++;
                    break;
                }
            }
            V(X->arithmetic);
            V(X->downArrow);
            //Person has completed getting into the lift--------

            //Person waiting to get out of the lift--------
            P(L->stopsem[des_idx]);
            L->step_cnt++;
            sprintf(msg,"%s[%d | Lift_%d] %s person got down .\n",lifttab[L->no-1] ,L->step_cnt,L->no, p.name);
            printf("%s", msg);

//            write(L->pipefd[1], msg, strlen(msg)+1);
            L->stops[des_idx]--;
            V(L->stopsem[des_idx]);
            //Person has got down from the lift------------
        }
        //================================================
        if(L->step_cnt > TOT_STEPS)
            break;

        sleep(1);
        //Person scheduling a new journey-------------------------------
        p.src = p.des;
        while (p.des == p.src){
            p.des = rand()%NFLOOR + 1;
        }


        int src_idx = p.src-1;
        FloorInfo *X = &(floors[src_idx]);

        P(X->arithmetic);
        if(p.des > p.src){ X->waitingToGoUp++;}
        else /* (p.des < p.src) */{ X->waitingToGoDown++;}
        printf("%s#[%s] | %d -> %d\n",personTab, name, p.src, p.des);
        V(X->arithmetic);
        //Person starts waiting at a floor to get into the lift----------
    }
    #pragma clang diagnostic pop

    P(exit_check_lock_semid);
    (*instance_cnt)--;
    V(exit_check_lock_semid);

    for(int i=0; i<NLIFT; i++){
        close(lifts[i].pipefd[1]);
    }
    printf("%s closed files\n", p.name);


    release(lifts, floors);
    return 0;
}

