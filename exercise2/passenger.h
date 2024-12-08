#include "ipc_utils.h"

void* passenger_thread() {
  
    sem_wait(&lifeboat_queue);
    
    printf("Passenger is waiting for a boat.\n");
    
    // Wait for available space on the boat (capacity control)
    sem_wait(&boat_space);
    
    // Boarding process
    printf("Passenger is boarding the boat.\n");
    sleep(1);  // Simulate time for boarding
    
    // After boarding, free up one space on the boat
    sem_post(&boat_space);
    
    // Finished boarding, so release the boat for others
    printf("Passenger has boarded the boat.\n");
    sem_post(&lifeboat_queue);
    
    return NULL;
}