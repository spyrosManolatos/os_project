#include "ipc_utils.h"
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

extern sem_t boats;
extern sem_t capacity;
extern sem_t mutex;
extern sem_t boarding;
extern int n_passengers;
extern int n_lboats;
extern int n_capacity;
extern int rescued;

void passenger_process(int id) {
    int boat_id = (id / n_capacity) % n_lboats;

    // Wait for boarding permission
    sem_wait(&boats);       // Wait for available boat
    sem_wait(&boarding);    // Wait for permission to board

    sem_wait(&mutex);
    printf("Passenger %d boarded lifeboat %d\n", id, boat_id);
    rescued++;
    if (rescued % n_capacity == 0 || rescued == n_passengers) {
        sem_post(&capacity);  // Signal boat is full
    }
    sem_post(&mutex);
    sem_post(&boats);     // Release boat for next passenger
}