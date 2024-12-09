#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>


#define MAX_LIFEBOATS 10

// Shared semaphore array
sem_t *lifeboat_capacity[MAX_LIFEBOATS];

// Function to initialize semaphores in shared memory
void initialize_lifeboats(int num_lifeboats) {
    key_t key = ftok("/tmp", 65); // Create a unique key for shared memory
    int shm_id = shmget(key, sizeof(sem_t) * num_lifeboats, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Attach to shared memory
    sem_t *shm_sems = (sem_t *)shmat(shm_id, NULL, 0);
    if (shm_sems == (sem_t *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphores for each lifeboat
    for (int i = 0; i < num_lifeboats; i++) {
        lifeboat_capacity[i] = &shm_sems[i];
    }
}

void *passenger_thread(void *id) {
    int passenger_id = *((int *)id);  // Get passenger ID
    int boarded = 0;

    while (!boarded) {
        for (int i = 0; i < MAX_LIFEBOATS; i++) {
            // Try to board a lifeboat
            if (sem_trywait(lifeboat_capacity[i]) == 0) {
                // Successfully boarded the lifeboat
                printf("Passenger %d has boarded lifeboat %d.\n", passenger_id, i + 1);
                sleep(1);  // Simulate time on the lifeboat

                // Wait for the lifeboat to complete its trip (no early sem_post here)
                boarded = 1;  // Passenger has boarded successfully
                break;
            }
        }

        // Retry if no lifeboat was available
        if (!boarded) {
            sleep(1);  // Wait before retrying
        }
    }

    return NULL;
}


int main() {
    int n_passengers;
    int n_lifeboats;

    printf("Enter the number of lifeboats: ");
    scanf("%d", &n_lifeboats);
    printf("Enter the number of passengers: ");
    scanf("%d", &n_passengers);

    // Initialize shared lifeboats semaphores
    initialize_lifeboats(n_lifeboats);

    // Create passenger threads
    pthread_t passengers[n_passengers];
    for (int i = 0; i < n_passengers; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;  // Passenger ID
        pthread_create(&passengers[i], NULL, passenger_thread, id);
    }

    // Wait for passenger threads to finish
    for (int i = 0; i < n_passengers; i++) {
        pthread_join(passengers[i], NULL);
    }

    printf("Passenger simulation completed.\n");
    return 0;
}
