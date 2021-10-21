#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include "structdefs.h"
#include <sys/stat.h>
#include "semun.h"

/*
...or in Standard C. From $6.8.6.1/1 of the C Language Standard

The identifier in a goto statement shall name a label located somewhere in the enclosing function.
 A goto statement shall not jump from outside the scope of an identifier
 having a variably modified type to inside the scope of that identifier.
*/

Person initPerson(int src, int des, char *name){
    Person P;
    P.src = src;
    P.des = des;
    P.name = name; //argv[5]
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



    int j;
    for(j=0; j<personCnt; j++){
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

    char name[2];
    name[0] = ('A'+((no-1)*5+j));
    name[1] = 0;

    printf("[%s] | %d -> %d \n", name, src, des);
    int stat = execl("./Person","./Person", src_str, des_str, shmidLifts_str, shmidFloors_str, name, NULL);
    if(stat == -1){
        int errsv = errno;
        printf("execl | %s | %d \n", __func__ , errsv);
        return;
    }

}

void forkLift(int no, LiftInfo* L, key_t shmidLifts, key_t shmidFloors){
    srand(time(NULL)*no);
    for(int i=0; i<no; i++){
        rand();
    }

    L->no = no;
    L->direction = (rand()%2)?1:-1;
    L->position = 1+(rand()%NFLOOR);
    L->peopleInLift = 0;
    int perm = 0777;

//    union semun zeroint;
//    zeroint.val = 0;
//    for(int i=0; i<NFLOOR; i++) {
//        L->stopsem[i] = semget(IPC_PRIVATE, 1, perm | IPC_CREAT);
//        semctl(L->stopsem[i], 0, SETVAL, zeroint);
//    }

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