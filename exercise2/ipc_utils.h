// ipc_utils.h
#include <semaphore.h>
#include <pthread.h>

// Semaphores
sem_t boat_semaphore; // Limits the number of passengers per boat
sem_t boarding_queue_semaphore; // Ensures orderly boarding

// Utility functions
void init_semaphores(int boat_capacity) {
    sem_init(&boat_semaphore, 0, boat_capacity);
    sem_init(&boarding_queue_semaphore, 0, 1); // Mutex-like semaphore
}

void destroy_semaphores() {
    sem_destroy(&boat_semaphore);
    sem_destroy(&boarding_queue_semaphore);
}