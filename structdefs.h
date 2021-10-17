#ifndef ASSIGNMENT6_STRUCTDEFS_H
#define ASSIGNMENT6_STRUCTDEFS_H

#include <sys/sem.h>

typedef struct FloorInfo FloorInfo;
typedef struct LiftInfo LiftInfo;
typedef struct Person Person;
typedef int semaphore;

#define NFLOOR 5
#define MAXPERSON 10 //at init per floor
#define NLIFT 3

struct Person{
    int src;
    int des;
};

struct FloorInfo{
    int waitingToGoUp;          /* #people waiting to go up */
    int waitingToGoDown;        /* #people waiting to go down */

    semaphore upArrow;          /* people going up wait on this */
    semaphore downArrow;        /* people going down wait on this */


    semaphore arithmetic;       /* lock for making updates to stops[i] atomic */
};

struct LiftInfo{
    int no;                     /* unique identifier for a lift */
    int position;               /* lift's current position */
    int direction; // {+1/-1}   /* add this varible allows changing floor */
    int peopleInLift;           /* #people in lift */
    int stops[NFLOOR];          /* #people(in lift) for each stop */

    //semaphore stopsem[NFLOOR];
                                /* #people in lift wait on one of these
                                    (unused and commented explained later)*/
};


Person initPerson(int src, int des);
void initFloor(int no, FloorInfo* F, key_t shmidLifts, key_t shmidFloors);
void initLift(int no, LiftInfo* L, key_t shmidLifts, key_t shmidFloors);




#endif //ASSIGNMENT6_STRUCTDEFS_H
