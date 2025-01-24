#include "ipc_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

extern sem_t* ready;
extern sem_t* pass_boarded;


void passenger_process(int id) {
    sem_wait(ready);
    printf("Passenger %d boarded\n", id);
    sem_post(pass_boarded);
    exit(0);
}