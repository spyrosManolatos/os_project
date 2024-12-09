#include <pthread.h>
#include <unistd.h> 
#include "ipc_utils.h"

// Global array of semaphores, one for each lifeboat
sem_t* lifeboat_capacity;
int* lifeboat_passenger_count;
pthread_mutex_t lifeboat_mutex;

// Mutex for output synchronization (to avoid race conditions when printing)
pthread_mutex_t print_mutex;

// Total number of lifeboats (global variable)
int num_lifeboats;

void* passenger(void* id) {
    int passenger_id = *((int*)id);  // Get passenger ID
    int boarded = 0;

    while (!boarded) {
        // Try to board an available lifeboat
        for (int i = 0; i < num_lifeboats; i++) {  // Iterating through all lifeboats
            // Wait for available space in the lifeboat
            if (sem_trywait(&lifeboat_capacity[i]) == 0) {
                // Board the lifeboat (critical section)
                pthread_mutex_lock(&print_mutex);
                printf("Passenger %d has boarded lifeboat %d.\n", passenger_id, i + 1);
                pthread_mutex_unlock(&print_mutex);

                // Simulate the passenger being on the lifeboat for a while
                sleep(1);  // Sleep for 1 second to simulate time spent on the lifeboat

                // Leave the lifeboat (signal the semaphore)
                pthread_mutex_lock(&print_mutex);
                printf("Passenger %d has left lifeboat %d.\n", passenger_id, i + 1);
                pthread_mutex_unlock(&print_mutex);

                // Signal that there's space available
                sem_post(&lifeboat_capacity[i]);

                boarded = 1;  // Passenger has boarded successfully
                break;  // Exit the loop after boarding one lifeboat
            }
        }

        // If no space was available, wait a bit before retrying
        if (!boarded) {
            sleep(1);  // Wait before retrying to board a lifeboat
        }
    }

    return NULL;
}

// Function to simulate the lifeboats working independently
void* lifeboat(void* id) {
    int lifeboat_id = *((int*)id);  // Get lifeboat ID

    while (1) {
        // Wait until the lifeboat is full (based on its capacity)
        sem_wait(&lifeboat_capacity[lifeboat_id]);

        // Lifeboat is full, so simulate a trip to the coast
        pthread_mutex_lock(&print_mutex);
        printf("Lifeboat %d is full and heading to the coast.\n", lifeboat_id + 1);
        pthread_mutex_unlock(&print_mutex);

        // Simulate the lifeboat trip to the coast (sleep for a while)
        sleep(3);  // Sleep for 3 seconds to simulate the trip

        // After dropping off passengers, the lifeboat returns and is ready for more passengers
        pthread_mutex_lock(&print_mutex);
        printf("Lifeboat %d has returned and is ready for more passengers.\n", lifeboat_id + 1);
        pthread_mutex_unlock(&print_mutex);
    }

    return NULL;
}

int main() {
    int lifeboat_capacity_value, num_passengers;

    // Prompt the user for the number of lifeboats, capacity, and passengers
    printf("Enter the number of lifeboats: ");
    scanf("%d", &num_lifeboats);

    printf("Enter the lifeboat capacity: ");
    scanf("%d", &lifeboat_capacity_value);

    printf("Enter the number of passengers: ");
    scanf("%d", &num_passengers);

    // Dynamically allocate memory for semaphores (one for each lifeboat)
    lifeboat_capacity = (sem_t*)malloc(num_lifeboats * sizeof(sem_t));

    // Initialize the semaphore for each lifeboat with the user-defined capacity
    for (int i = 0; i < num_lifeboats; i++) {
        semaphore_init(&lifeboat_capacity[i], lifeboat_capacity_value);  // Initialize lifeboat semaphore
    }

    // Initialize the mutex for synchronized output
    pthread_mutex_init(&print_mutex, NULL);

    // Create threads for lifeboats
    pthread_t* lifeboats = (pthread_t*)malloc(num_lifeboats * sizeof(pthread_t));
    int* lifeboat_ids = (int*)malloc(num_lifeboats * sizeof(int));

    for (int i = 0; i < num_lifeboats; i++) {
        lifeboat_ids[i] = i;
        if (pthread_create(&lifeboats[i], NULL, lifeboat, &lifeboat_ids[i]) != 0) {
            perror("Failed to create lifeboat thread");
            exit(EXIT_FAILURE);
        }
    }

    // Create threads for passengers
    pthread_t* passengers = (pthread_t*)malloc(num_passengers * sizeof(pthread_t));
    int* passenger_ids = (int*)malloc(num_passengers * sizeof(int));

    for (int i = 0; i < num_passengers; i++) {
        passenger_ids[i] = i + 1;
        if (pthread_create(&passengers[i], NULL, passenger, &passenger_ids[i]) != 0) {
            perror("Failed to create passenger thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all passengers to finish boarding
    for (int i = 0; i < num_passengers; i++) {
        pthread_join(passengers[i], NULL);
    }

    // Clean up
    for (int i = 0; i < num_lifeboats; i++) {
        semaphore_destroy(&lifeboat_capacity[i]);
    }

    free(lifeboat_capacity);
    free(lifeboats);
    free(lifeboat_ids);
    free(passengers);
    free(passenger_ids);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}
