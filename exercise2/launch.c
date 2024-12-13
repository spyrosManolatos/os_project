#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>

sem_t ready;
sem_t boats;
sem_t capacity;
sem_t ready;
int current_lifeboat = 0;

void init_semaphores(int n_capacity, int n_lboats) {
    sem_init(&capacity, 0, n_capacity);
    sem_init(&boats, 0, n_lboats);
    sem_init(&ready, 0,0);
    sem_init(&ready, 0, 1);
}


void passenger_process(int id) {
    sem_wait(&boats);
    int lifeboat_id = current_lifeboat;
    printf("Passenger %d is boarding a lifeboat  %d.\n", id, lifeboat_id);
    sem_wait(&capacity);
    printf("Passenger %d has boarded a lifeboat  %d.\n", id, lifeboat_id);
    if(sem_getvalue(&capacity,NULL) == 0){
        sem_post(&ready);
    }
}

void lifeboat_process(int id) {
    printf("Lifeboat %d is waiting for passengers.\n", id);
    current_lifeboat = id;
    sem_wait(&ready);
    printf("Lifeboat %d has boarded passengers.\n", id);
    printf("Lifeboat %d returned from coast.\n", id);
    for (int i = 0; i < sem_getvalue(&capacity, NULL); i++) {
        sem_post(&capacity);
    }
    sem_post(&boats);
}

int main() {

    int n_passengers;
    int n_boats;
    int n_capacity;

    // Take input values
    printf("Enter the number of passengers: ");
    scanf("%d", &n_passengers);
    printf("Enter the number of lifeboats: ");
    scanf("%d", &n_boats);
    printf("Enter the capacity of each lifeboat: ");
    scanf("%d", &n_capacity);

    // Initialize semaphores
    init_semaphores(n_capacity, n_boats);

    // Create lifeboat processes
    for(int i = 0; i < n_boats; i++){
         if(fork() == 0) {
            lifeboat_process(i);
        }
    }
      // Create passenger processes
    for(int i = 0; i < n_passengers; i++){
         if(fork() == 0) {
            passenger_process(i);
        }
    }
    return 0;
}