#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "structdefs.h"

int main(int argc, char **argv){
    if (argc != 3){
        printf("'Person' process executed with incorrect no. of parameters\n");
        return 1;
    }

    int src = atoi(argv[1]);
    int des = atoi(argv[2]);

    printf("%s %d %d\n",argv[0], src, des);
    return 0;
}

