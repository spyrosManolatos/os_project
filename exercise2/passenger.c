///////////////////////////////////
// Manolatos Spyridon, 1104802
// Maroudas Andreas-Erikos, 1100617
// Kirkinis Manousos, 1100590
// Chrysanthakopoulos Nikolaos, 1100760
////////////////////////////////////
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