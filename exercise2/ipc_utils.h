#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* passenger_thread(); 

// Semaphore declarations
extern sem_t boat_space;
extern sem_t passenger_queue;
extern sem_t lifeboat_queue;
extern pthread_mutex_t queue_lock;

// Initialization function
void init_resources(int boat_capacity) {
    sem_init(&boat_space, 0, boat_capacity); // Semaphore for available seats on the lifeboat
    sem_init(&passenger_queue, 0, 0);        // Semaphore for passengers in the queue
    pthread_mutex_init(&queue_lock, NULL);   // Mutex for queue synchronization
}

// Cleanup function
void cleanup_resources() {
    sem_destroy(&boat_space);
    sem_destroy(&passenger_queue);
    pthread_mutex_destroy(&queue_lock);
}
