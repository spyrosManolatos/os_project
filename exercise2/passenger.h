#include "ipc_utils.h"

void* passenger_thread() {
  
    printf("A passenger is waiting to board.\n");

    // Wait to enter the queue
    sem_wait(&passenger_queue);
    printf("A passenger is in the queue.\n");

    // Wait for space on the boat
    sem_wait(&boat_space);
    printf("A passenger is boarding the lifeboat.\n");

    // Simulate boarding time
    sleep(1);

    printf("A passenger has boarded the lifeboat.\n");
    return NULL;
}