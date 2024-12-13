 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include "ipc_utils.h"

sem_t boats;         // Controls available boats
sem_t capacity;      // Controls boat capacity
sem_t mutex;         // Protects counter and print statements
sem_t boarding;      // Controls passenger boarding
int n_passengers;    // Total passengers to rescue
int n_lboats;        // Number of lifeboats
int n_capacity;      // Capacity per boat
int lpassengers;

void passenger_process(int id) {
    int boat_id = (id / n_capacity) % n_lboats;
   
    semaphore_wait(&boats);       // Wait for available boat
    semaphore_wait(&boarding);    // Wait for permission to board
    
    lpassengers = lpassengers + 1;
    semaphore_wait(&mutex);       // Start of critical section
    printf("Passenger %d boarded lifeboat %d\n", id, boat_id);
    if (lpassengers  == n_capacity) {
        semaphore_signal(&capacity);  // Signal boat is full
    }
    semaphore_signal(&mutex);      // End of critical section
    exit(0);
}

void lifeboat_process(int id) {

    while (1) {
        printf("Lifeboat %d ready for boarding\n", id);

        // Allow n_capacity passengers to board
        for (int i = 0; i < n_capacity; i++) {
            semaphore_signal(&boarding);
        }

        semaphore_signal(&capacity);  // Wait until boat is full

        semaphore_wait(&mutex);     // Start of critical section
        if (lpassengers>= n_capacity) {
            semaphore_signal(&mutex);  // End critical section
            break;
        }
        int current_load = (lpassengers% n_capacity == 0) ? n_capacity : (lpassengers% n_capacity);
        printf("Lifeboat %d is full with %d passengers\n", id, current_load);
        semaphore_signal(&mutex);   // End of critical section

        sleep(1);
        printf("Lifeboat %d reached the coast\n", id);
        sleep(1);
        printf("Lifeboat %d returned to ship\n", id);

        // Reset capacity for next journey
        for (int i = 0; i < current_load; i++) {
            semaphore_signal(&capacity);
        }
    }
    exit(0);
}

int main() {
    printf("Insert the number of passengers: ");
    scanf("%d", &n_passengers);
    printf("Insert the number of lifeboats: ");
    scanf("%d", &n_lboats);
    printf("Insert lifeboat capacity: ");
    scanf("%d", &n_capacity);

    semaphore_init(&boats, n_lboats);  // Initialize with number of boats
    semaphore_init(&capacity, 1);      // Start with  capacity signal meaning empty boat
    semaphore_init(&mutex, 1);         // Binary semaphore for mutual exclusion
    semaphore_init(&boarding, 1);      // Start with  boarding signal meaning passenger is boarding

    // Create passenger processes
    for (int i = 0; i < n_passengers; i++) {
        if (fork() == 0) {
            passenger_process(i);
        }
    }

    // Create lifeboat processes
    for (int i = 0; i < n_lboats; i++) {
        if (fork() == 0) {
            lifeboat_process(i);
        }
    }

    // Wait for all processes to complete
    for (int i = 0; i < n_passengers + n_lboats; i++) {
        wait(NULL);
    }

    printf("All passengers rescued!\n");

    // Cleanup semaphores
    semaphore_destroy(&boats);
    semaphore_destroy(&capacity);
    semaphore_destroy(&mutex);
    semaphore_destroy(&boarding);

    return 0;
}
