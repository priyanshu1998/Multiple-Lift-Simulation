#ifndef ASSIGNMENT6_STRUCTDEFS_H
#define ASSIGNMENT6_STRUCTDEFS_H

#include <sys/sem.h>

typedef struct FloorInfo FloorInfo;
typedef struct LiftInfo LiftInfo;
typedef struct Person Person;
typedef int semaphore;

#define NFLOOR 5
#define MAXPERSON 2 //at per floor

// NFLOOR * MAXPERSON should always <= 26 as PERSON ARE NAMED A,B,C,...
#define NLIFT 2

#define TOT_STEPS 50

struct FloorInfo{
    int waitingToGoUp;          /* #people waiting to go up */
    int waitingToGoDown;        /* #people waiting to go down */

    semaphore upArrow;          /* people going up wait on this */
    semaphore downArrow;        /* people going down wait on this */


    /* Added for cases when to lift going is same direction come at the floor */
    semaphore arithmetic;     /* lock for making updates to F.waitingToGoDown / F.waitingToGoUp  atomic */
    /*------------------------------------------------------------------------*/};

struct LiftInfo{
    int no;                     /* unique identifier for a lift */
    int position;               /* lift's current position */
    int direction; // {+1/-1}   /* add this varible allows changing floor */
    int peopleInLift;           /* #people in lift */
    int stops[NFLOOR];          /* #people(in lift) for each stop */

    semaphore stopsem[NFLOOR];/* #people in lift wait on one of these */
    int step_cnt;
};

void initFloorForkPersons(int no, FloorInfo* F, key_t shmidLifts, key_t shmidFloors,int instance_cnt_shmid, int exit_check_lock_semid);
void forkLift(int no, LiftInfo* L, key_t shmidLifts, key_t shmidFloors, int instance_cnt_shmid, int exit_check_lock_semid);


#endif //ASSIGNMENT6_STRUCTDEFS_H
