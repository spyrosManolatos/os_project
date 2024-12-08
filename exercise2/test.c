#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t boat_space;        // Semaphore for managing available boat spaces (capacity)
sem_t passenger_queue;   // Semaphore for managing the queue of passengers
sem_t lifeboat_queue;    // Semaphore for managing available boats

int capacity; // boat capacity
int n_boats;  // number of boats

void* passenger_thread(void* arg) {
    // Wait for an available boat
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

// This function simulates a lifeboat's journey, including when it leaves and returns.
void* lifeboat_thread(void* arg) {
    int passengers_on_board = 0;
    
    printf("Lifeboat is ready for boarding.\n");

    // Load passengers until the boat reaches full capacity
    while (1) {
        // Wait for passengers to board
        sem_wait(&passenger_queue);  // Wait for a passenger to be queued
        
        // Boarding a passenger
        sem_wait(&boat_space);  // Wait for available space on the boat
        passengers_on_board++;
        printf("Passenger %d is boarding. Boat has %d/%d passengers.\n", passengers_on_board, passengers_on_board, capacity);
        
        // If the boat is full, signal that it's leaving
        if (passengers_on_board == capacity) {
            printf("Boat is full and leaving for the shore.\n");
            sleep(1); // Simulate the boat leaving and going to shore
            printf("Boat has returned from the shore.\n");
            
            // Reset and allow new passengers to board
            passengers_on_board = 0;
            sem_post(&boat_space);  // Allow the boat to be used again
        }
    }

    return NULL;
}

void init_resources(int cap, int boats) {
    capacity = cap;
    n_boats = boats;
    
    // Initialize the semaphores
    sem_init(&boat_space, 0, capacity);  // Each boat can hold 'capacity' number of passengers
    sem_init(&lifeboat_queue, 0, n_boats); // Total 'n_boats' are available for boarding
    sem_init(&passenger_queue, 0, 0); // No passengers initially
}

void cleanup_resources() {
    // Clean up semaphores
    sem_destroy(&boat_space);
    sem_destroy(&lifeboat_queue);
    sem_destroy(&passenger_queue);
}

int main() {
    int n_passengers;
    
    printf("Enter the number of passengers in the ship:");
    scanf("%d", &n_passengers);
    printf("Enter the number of life boats:");
    scanf("%d", &n_boats);
    printf("Enter the capacity of each life boat:");
    scanf("%d", &capacity);

    printf("Initializing lifeboat simulation with %d passengers, %d boats and a boat capacity of %d...\n", n_passengers, n_boats, capacity);

    init_resources(capacity, n_boats);

    // Signal passengers to start boarding
    for (int i = 0; i < n_passengers; i++) {
        sem_post(&passenger_queue);  // Allow passengers to queue up for boats
    }

    printf("All passengers are queued for boarding.\n");

    pthread_t passengers[n_passengers];
    pthread_t boats[n_boats];

    // Create lifeboat threads
    for (int i = 0; i < n_boats; i++) {
        pthread_create(&boats[i], NULL, lifeboat_thread, NULL);
    }

    // Create passenger threads
    for (int i = 0; i < n_passengers; i++) {
        pthread_create(&passengers[i], NULL, passenger_thread, NULL);
        sleep(1); // Simulate staggered arrival of passengers
    }

    // Wait for boat threads to finish
    for (int i = 0; i < n_boats; i++) {
        pthread_join(boats[i], NULL);
    }

    // Wait for passenger threads to finish
    for (int i = 0; i < n_passengers; i++) {
        pthread_join(passengers[i], NULL);
    }

    cleanup_resources();

    printf("Lifeboat simulation completed.\n");
    return EXIT_SUCCESS;
}
