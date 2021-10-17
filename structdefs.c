#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include "structdefs.h"
#include <sys/stat.h>
#include "semun.h"

#define INITLOG "initlogs.txt"



Person initPerson(int src, int des){
    Person P;
    P.src = src;
    P.des = des;
    return P;
}

/*
 * initFloorForkPersons function creates random no. of person processes and
 * according to the source and destination floor no. of a person
 * (decided at runtime of Person Process)
 * waitingToGoUp and waitingToGoDown is updated.
 *
 * All person processes after deciding their source and destination.
 * wait till all person on a floor have decided their destination.
 */

void initFloorForkPersons(int no, FloorInfo *F, key_t shmidLifts, key_t shmidFloors){
    srand(time(NULL)*no);
    int personCnt = rand()%(MAXPERSON + 1); //personCnt belongs to {0, 1, 2, 3,..., MAXPERSON}
    printf("%d people at floor %d\n", personCnt, no);

    int src = -1, des = -1;
    char src_str[10] = "", des_str[10] = "";
    char shmidLifts_str[24], shmidFloors_str[24];

//    printf("#%d|%s\n", shmidLifts, shmidLifts_str);

    // Deviating from implementation of sample code(of slides)

    // technically semctl accepts lvalue as a parameter(as seen in sample code)
    // but as far as I know an unnamed temporary variable is created
    // during the call. So an instance of that variable is created and used
    // I don't have much idea about how internally unions are handled.

    // but I am guessing if union is not defined for a system. program will not compile.
    // hence as per documentation using the defined way of using semctl for SETVAL command.

    union semun zeroint;
    zeroint.val = 0;

    F->upArrow = semget(IPC_PRIVATE,1, S_IRWXU|S_IRWXO|S_IRWXO|IPC_CREAT);
    semctl(F->upArrow, 0, SETVAL, zeroint);

    F->downArrow = semget(IPC_PRIVATE, 1, S_IRWXU|S_IRWXO|S_IRWXO|IPC_CREAT);
    semctl(F->downArrow, 0, SETVAL, zeroint);

    F->arithmetic = semget(IPC_PRIVATE, 1, S_IRWXU|S_IRWXO|S_IRWXO|IPC_CREAT);
    semctl(F->arithmetic, 0, SETVAL, zeroint);


    for(int j=0; j<personCnt; j++){
        int pid = fork();
//        printf("#%d|%s\n", shmidLifts, shmidLifts_str);

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

    child_label :
    sprintf(src_str, "%d", src);
    sprintf(des_str, "%d", des);
    sprintf(shmidLifts_str, "%d", shmidLifts);
    sprintf(shmidFloors_str, "%d", shmidFloors);

    int stat = execl("./Person","./Person", src_str, des_str, shmidLifts_str, shmidFloors_str, NULL);
    if(stat == -1){
        int errsv = errno;
        printf("execl | %s | %d \n", __func__ , errsv);
        return;
    }

}

void forkLift(int no, LiftInfo* L, key_t shmidLifts, key_t shmidFloors){
    srand(time(NULL)*no);
    for(int i=0; i<=no; i++){
        rand();
    }

    L->no = no;
    L->position = 1+rand()%NFLOOR;
    L->direction = rand()%2;
    L->peopleInLift = 0;

    for(int i=0; i<NFLOOR; i++){
        L->stops[i] = 0;
    }

    char shmidLifts_str[24], shmidFloors_str[24];
    char no_str[8];

    sprintf(no_str, "%d", no);
    sprintf(shmidLifts_str, "%d", shmidLifts);
    sprintf(shmidFloors_str, "%d", shmidFloors);

    int pid = fork();
    if(pid == 0)
    {
        int stat = execl("./Lift","./Lift", no_str, shmidLifts_str, shmidFloors_str, NULL);
        if(stat == -1){
            int errsv = errno;
            printf("execl | %s | %d \n", __func__ , errsv);
            return;
        }
    }
    return;
}