#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>

#define MAX_LIFEBOATS 10

// Shared semaphore array
sem_t *lifeboat_capacity[MAX_LIFEBOATS];

// Struct to pass data to threads
typedef struct {
    int lifeboat_id;
    int capacity;
} LifeboatData;

// Function to initialize semaphores in shared memory
void initialize_lifeboats(int num_lifeboats, int capacity) {
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
        sem_init(&shm_sems[i], 1, capacity);
        lifeboat_capacity[i] = &shm_sems[i];
    }
}

void *lifeboat_thread(void *arg) {
    LifeboatData *data = (LifeboatData *)arg; // Cast argument to LifeboatData
    int lifeboat_id = data->lifeboat_id;
    int capacity = data->capacity;

    while (1) {
        int passengers_boarded = 0;

        // Wait until the lifeboat is full
        printf("Lifeboat %d is ready to board passengers.\n", lifeboat_id + 1);
        while (passengers_boarded < capacity) {  // Wait for passengers
            sem_wait(lifeboat_capacity[lifeboat_id]);  // Passenger boards
            passengers_boarded++;
        }

        // Lifeboat is full, heading to the coast
        printf("Lifeboat %d is full and heading to the coast.\n", lifeboat_id + 1);
        sleep(3);  // Simulate trip

        // Lifeboat returns and resets capacity
        printf("Lifeboat %d has returned and is ready for passengers.\n", lifeboat_id + 1);
        for (int i = 0; i < passengers_boarded; i++) {
            sem_post(lifeboat_capacity[lifeboat_id]);  // Reset semaphore
        }
    }
}


int main() {
    int n_lifeboats;
    int capacity;

    printf("Enter the number of lifeboats: ");
    scanf("%d", &n_lifeboats);
    printf("Enter the capacity of each lifeboat: ");
    scanf("%d", &capacity);

    printf("Initializing lifeboat simulation with %d boats, each with a capacity of %d...\n", n_lifeboats, capacity);

    // Initialize lifeboats with shared semaphores
    initialize_lifeboats(n_lifeboats, capacity);

    // Create lifeboat threads
    for (int i = 0; i < n_lifeboats; i++) {
        LifeboatData *data = malloc(sizeof(LifeboatData));
        data->lifeboat_id = i;
        data->capacity = capacity;

        if (fork() == 0) {  // Create a child process for each lifeboat
            lifeboat_thread(data);
            exit(0);  // Exit the child process after finishing the task
        }
    }

    // Wait for lifeboat processes to finish
    while (wait(NULL) > 0);

    printf("Lifeboat simulation completed.\n");
    return 0;
}
