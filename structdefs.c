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

/*
 * initFloorForkPersons function creates random no. of person processes and
 * according to the source and destination floor no. of a person
 * (decided at runtime of Person Process)
 * waitingToGoUp and waitingToGoDown is updated.
 *
 * All person processes after deciding their source and destination.
 * wait till all person on a floor have decided their destination.
 */

void initFloorForkPersons(int no, FloorInfo *F, key_t shmidLifts, key_t shmidFloors,int instance_cnt_shmid, int exit_check_lock_semid){
    srand(time(NULL)*no);
//    printf("%d people at floor %d\n", personCnt, no);

    int src = -1, des = -1;
    char src_str[10] = "", des_str[10] = "";
    char shmidLifts_str[24], shmidFloors_str[24];
    char name[2];
    char instance_cnt_shmid_str[24], exit_check_lock_semid_str[24];

//    printf("#%d|%s\n", shmidLifts, shmidLifts_str);



    int j;
    for(j=0; j<MAXPERSON; j++){
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

    return;

    child_label :
    sprintf(src_str, "%d", src);
    sprintf(des_str, "%d", des);
    sprintf(shmidLifts_str, "%d", shmidLifts);
    sprintf(shmidFloors_str, "%d", shmidFloors);
    sprintf(instance_cnt_shmid_str, "%d", instance_cnt_shmid);
    sprintf(exit_check_lock_semid_str, "%d", exit_check_lock_semid);

    name[0] = ('A'+((no-1)*MAXPERSON+j));
    name[1] = 0;


    int stat = execl("./Person","./Person", src_str, des_str, shmidLifts_str, shmidFloors_str,
                     name,instance_cnt_shmid_str, exit_check_lock_semid_str, NULL);
    if(stat == -1){
        int errsv = errno;
        printf("execl | %s | %d \n", __func__ , errsv);
        return;
    }

}

void forkLift(int no, LiftInfo* L, key_t shmidLifts, key_t shmidFloors,int instance_cnt_shmid, int exit_check_lock_semid){
    srand(time(NULL)*no);
    for(int i=0; i<no; i++){
        rand();
    }

    L->no = no;
    L->direction = (rand()%2)?1:-1;
    L->position = 1+(rand()%NFLOOR);
    L->peopleInLift = 0;
    L->step_cnt = 0;

    /* SPECIAL CASE
     * If we have two lift in order to minimize waiting time run lifts in opposite directions.
     */
    if(NLIFT == 2){
        if(no == 1){
            L->direction = 1;
            L->position = 1;
        }
        else/*no == 2*/{
            L->direction = -1;
            L->position = NFLOOR;
        }
    }

    for(int i=0; i<NFLOOR; i++){
        L->stops[i] = 0;
    }

    char shmidLifts_str[24], shmidFloors_str[24];
    char instance_cnt_shmid_str[24], exit_check_lock_semid_str[24];

    char no_str[8];

    sprintf(no_str, "%d", no);
    sprintf(shmidLifts_str, "%d", shmidLifts);
    sprintf(shmidFloors_str, "%d", shmidFloors);
    sprintf(instance_cnt_shmid_str, "%d", instance_cnt_shmid);
    sprintf(exit_check_lock_semid_str, "%d", exit_check_lock_semid);

    int pid = fork();
    if(pid == 0)
    {
        int stat = execl("./Lift","./Lift", no_str, shmidLifts_str, shmidFloors_str,
                         instance_cnt_shmid_str, exit_check_lock_semid_str, NULL);
        if(stat == -1){
            int errsv = errno;
            printf("execl | %s | %d \n", __func__ , errsv);
            return;
        }
    }
    return;
}