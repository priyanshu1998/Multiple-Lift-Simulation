#include <stdio.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

#include "structdefs.h"


#define P(s) semop(s, &pop, 1)              /* pop is the structure we pass for doing the P(s) operation */
#define V(s) semop(s, &vop, 1)              /* vop is the structure we pass for doing the V(s) operation */


void initFloors(){
    int perm = S_IRWXU | S_IRWXG | S_IRWXO;
    int shmid = shmget(IPC_PRIVATE, NFLOOR*sizeof (FloorInfo), perm | IPC_CREAT);

    if(shmid == -1){
        int errsv = errno;
        printf("Error: shmget | %s | %d \n", __func__ ,errsv);
    }

    FloorInfo *floors = (FloorInfo*)shmat(shmid, NULL, 0);
    if((void*)floors == (void*)-1){
        int errsv = errno;
        printf("Error: shmat | %s | %d \n", __func__ , errsv);
    }

    for(int i=0; i<NFLOOR; i++){
        initFloor(i+1, floors+i);
    }

    return;
}


int main() {
    initFloors();

    while(wait(-1));
    printf("Hello, World!\n");
    return 0;
}
