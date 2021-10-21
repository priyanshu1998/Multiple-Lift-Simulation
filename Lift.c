#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>



#include "structdefs.h"
#include "ipcwrappers.h"

int main(int argc, char **argv){
    if(argc != 4){
        printf("'Lift' Process executed with incorrect no. of parameters\n");
        return 1;
    }

    int liftno = atoi(argv[1]);
    int shmidLifts = atoi(argv[2]);
    int shmidFloors = atoi(argv[3]);

    LiftInfo *lifts = NULL;
    FloorInfo *floors = NULL;

//    char filename[]
//    strcat()
//    FILE *fp = fopen("")

    init(shmidLifts, shmidFloors, &lifts, &floors);

    LiftInfo *L = &(lifts[liftno-1]); //as liftno is 1 indexed
    printf("[0 | Lift_%d] | initial floor: %d\n", L->no, L->position);

//    printf("# %s %d %d %d\n", argv[0], liftno, shmidLifts, shmidFloors);

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        /* CASE WHEN LIFT IS GOING UP */
        if(L->direction == 1) {
//            printf("Lift %d is on %d[UP].\n",L->no, L->position);
            int F = L->position - 1;// floor in array are 0 indexed.
            FloorInfo *X = &(floors[F]);

            // Releasing lock for those who want to get down =======
            V(L->stopsem[F]);
            while (1) {
                P(L->stopsem[F]);
                if (L->stops[F] == 0) { break; }
                V(L->stopsem[F]);
                sleep(LIFT_SLEEP_T);
            }

            // Releasing lock for those who want to get up =========
            V(X->upArrow);
            while (1) {
                P(X->upArrow);
                P(X->arithmetic);

                if (X->waitingToGoUp == 0) {
                    if(L->position == NFLOOR){
                        L->direction = -1;
                        sleep(END_STOPTIME);
                    }
                    L->position += L->direction;
                    L->step_cnt++;

                    if(L->direction == 1) {printf("[%d | Lift_%d] move up to %d.\n",L->step_cnt,L->no, L->position);}
                    else{ printf("[%d | Lift_%d] move down to %d.\n",L->step_cnt,L->no, L->position);}

                    break;
                }
                V(X->arithmetic);
                V(X->upArrow);
                sleep(LIFT_SLEEP_T);
            }
            V(X->arithmetic); // X.arithmetic was 1 before 'P(X.upArrow) line'
            // release X.arithmetic lock to maintain the state.
//            printf("Lift %d moved up.\n",L->no);

        }
        /******************END OF CASE******************/



        /* CASE WHEN LIFT IS GOING DOWN */
        else/*L.direction == -1*/{
//            printf("Lift %d is on %d[DOWN].\n",L->no, L->position);
            int F = L->position - 1;// floor in array are 0 indexed.
            FloorInfo *X = &(floors[F]);

            // Releasing lock for those who want to get down =======
            V(L->stopsem[F]);
            while (1) {
                P(L->stopsem[F]);
                if (L->stops[F] == 0) { break; }
                V(L->stopsem[F]);
                sleep(LIFT_SLEEP_T);
            }


            // Releasing lock for those who want to get up =========
            V(X->downArrow);
            while (1) {
                P(X->downArrow);
                P(X->arithmetic);
                if (X->waitingToGoDown == 0) {
                    if(L->position == 1){
                        L->direction = 1;
                        sleep(END_STOPTIME);
                    }
                    L->position += L->direction;

                    L->step_cnt++;
                    if(L->direction == 1) {printf("[%d | Lift_%d] move up to %d.\n",L->step_cnt,L->no, L->position);}
                    else{ printf("[%d | Lift_%d] move down to %d.\n",L->step_cnt,L->no, L->position);}

                    break;
                }
                V(X->arithmetic);
                V(X->downArrow);
                sleep(LIFT_SLEEP_T);
            }
            V(X->arithmetic);
        }
        if(L->step_cnt > TOT_STEPS)break;
        /******************END OF CASE******************/
    }
    #pragma clang diagnostic pop

    release(lifts, floors);
    return 0;
}

