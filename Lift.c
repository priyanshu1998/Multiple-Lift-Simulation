#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>

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

    init(shmidLifts, shmidFloors, &lifts, &floors);

    LiftInfo L = lifts[liftno-1]; //as liftno is 1 indexed

//    printf("# %s %d %d %d\n", argv[0], liftno, shmidLifts, shmidFloors);

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "EndlessLoop"
    while(1){
        /* CASE WHEN LIFT IS GOING UP */
        if(L.direction == 1) {
            int F = L.position - 1;// floor in array are 0 indexed.
            FloorInfo X = floors[F];

            // Releasing lock for those who want to get down =======
            P(L.stopsem[F]);
            while (1) {
                V(L.stopsem[F]);
                if (L.stops[F] == 0) { break; }
                P(L.stopsem[F]);
                sleep(1);
            }

            // Releasing lock for those who want to get up =========
            P(X.upArrow);
            while (1) {
                V(X.upArrow);
                V(X.arithmetic);
                if (X.waitingToGoUp == 0) {
                    if(L.position == NFLOOR){
                        L.direction = -1;
                    }

                    L.position += L.direction;
                    break;
                }
                P(X.arithmetic);
                P(X.upArrow);
                sleep(1);
            }
            P(X.arithmetic); // X.arithmetic was 1 before 'P(X.upArrow) line'
            // release X.arithmetic lock to maintain the state.
        }
        /******************END OF CASE******************/



        /* CASE WHEN LIFT IS GOING DOWN */
        else/*L.direction == -1*/{
            int F = L.position - 1;// floor in array are 0 indexed.
            FloorInfo X = floors[F];

            // Releasing lock for those who want to get down =======
            P(L.stopsem[F]);
            while (1) {
                V(L.stopsem[F]);
                if (L.stops[F] == 0) { break; }
                P(L.stopsem[F]);
                sleep(1);
            }
            V(L.stopsem[F]);

            // Releasing lock for those who want to get up =========
            P(X.downArrow);
            while (1) {
                V(X.downArrow);
                V(X.arithmetic);
                if (X.waitingToGoDown == 0) {
                    if(L.position == 1){
                        L.direction = 1;
                    }
                    L.position += L.direction;
                    break;
                }
                P(X.arithmetic);
                P(X.downArrow);
                sleep(1);
            }
            P(X.arithmetic);
        }
        /******************END OF CASE******************/
    }
    #pragma clang diagnostic pop

    onCtrlC:
    release(lifts, floors);
    return 0;
}

