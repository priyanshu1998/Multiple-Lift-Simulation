#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "structdefs.h"

#define INITLOG "initlogs.txt"

void initPerson(int src, int des){
    Person P;
    P.src = src;
    P.des = des;
}

/*
 * initFloor function creates random no. of person processes and
 * according to the source and destination floor no. of a person
 * (decided at runtime of Person Process)
 * waitingToGoUp and waitingToGoDown is updated.
 *
 * All person processes after deciding their source and destination.
 * wait till all person on a floor have decided their destination.
 */

void initFloor(int no, FloorInfo *F){
    srand(time(NULL)*no);
    int personCnt = rand()%(MAXPERSON + 1); //personCnt belongs to {0, 1, 2, 3,..., MAXPERSON}
    printf("%d people at floor %d\n", personCnt, no);
    int src = -1, des = -1;
    char src_str[10] = "", des_str[10] = "";


    for(int j=0; j<personCnt; j++){
        int pid = fork();
        if(pid == 0){
            src = no;

            for(int k=0; k<=j; k++) {
                rand();
            }

            des = 1 + rand()%NFLOOR;
            while(src == des){
                des = 1 + rand()%NFLOOR;
            }


            //There is
            // only one fork() -> increment -> repeat(loop)
            // i.e increment before next fork hence there is no racing.

            if (des > src){
                (F->waitingToGoUp)++;
            }
            else{
                (F->waitingToGoDown)++;
            }
            goto child_label;
        }
    }

//    while(wait(-1));
    return;

    child_label:

    sprintf(src_str, "%d", src);
    sprintf(des_str, "%d", des);

    execl("./person","./person", src_str, des_str, NULL);
    return;
}
