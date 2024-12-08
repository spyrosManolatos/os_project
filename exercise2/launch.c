#include "passenger.h"

sem_t boat_space;
sem_t passenger_queue;
sem_t lifeboat_queue;

pthread_mutex_t queue_lock;

void* lifeboat_thread() {
    printf("A lifeboat is ready for boarding.\n");
    return NULL;
}

int main() {
    
   int n_passengers;
   int n_boats;
   int capacity;
    
    printf("Enter the number of passengers in the ship:");
    scanf("%d", &n_passengers);
    printf("Enter the number of life boats:");
    scanf("%d", &n_boats);
    printf("Enter the capacity of each life boat:");
    scanf("%d", &capacity);

    printf("Initializing lifeboat simulation with %d passengers, %d boats and a boat capacity of %d...\n", n_passengers, n_boats, capacity);

    init_resources(capacity);

    // Signal passengers to start boarding
    for (int i = 0; i < n_passengers; i++) {
        sem_post(&lifeboat_queue);  // Allow passengers to queue up for boats
    }

    printf("All passengers are queued for boarding.\n");

    pthread_t passengers[n_passengers];
    pthread_t boats[n_boats];

    // Create passenger threads
    for (int i = 0; i < n_passengers; i++) {
        pthread_create(&passengers[i], NULL, passenger_thread, NULL);
        sleep(1); // Simulate staggered arrival of passengers
    }

    // Create lifeboat threads
    for (int i = 0; i < n_boats; i++) {
        pthread_create(&boats[i], NULL, lifeboat_thread, NULL);
    }

    // Wait for passenger threads to finish
    for (int i = 0; i < n_passengers; i++) {
        pthread_join(passengers[i], NULL);
    }

    // Wait for boat threads to finish
    for (int i = 0; i < n_boats; i++) {
        pthread_join(boats[i], NULL);
    }

    cleanup_resources();

    printf("Lifeboat simulation completed.\n");
    return EXIT_SUCCESS;
}

