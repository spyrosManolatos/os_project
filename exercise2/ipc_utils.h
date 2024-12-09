// ipc_utils.h: Helper utilities for semaphore management

#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Function to initialize a semaphore
void semaphore_init(sem_t *sem, int value) {
    if (sem_init(sem, 0, value) != 0) {
        printf("Failed to initialize semaphore");
        exit(1);
    }
}

// Function to wait (decrement) a semaphore
 void semaphore_wait(sem_t *sem) {
    if (sem_wait(sem) != 0) {
        printf("Failed to wait on semaphore");
        exit(1);
    }
}

// Function to signal (increment) a semaphore
void semaphore_signal(sem_t *sem) {
    if (sem_post(sem) != 0) {
        printf("Failed to signal semaphore");
        exit(1);
    }
}

// Function to destroy a semaphore
 void semaphore_destroy(sem_t *sem) {
    if (sem_destroy(sem) != 0) {
        printf("Failed to destroy semaphore");
        exit(1);
    }
}

#endif // IPC_UTILS_H
