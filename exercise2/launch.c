#include <fcntl.h> // For O_CREAT and O_EXCL
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ipc_utils.h"

#define SEM_READY "/sem_ready"
#define SEM_BOATS "/sem_boats"
#define SEM_CAPACITY "/sem_capacity"
#define SEM_PASS_BOARDED "/sem_pass_boarded"

int current_lifeboat = 2;
int passengers_boarded = 0;
int n_passengers = 30;
int n_boats = 5;
int n_capacity = 4;

sem_t *ready;
sem_t *boats;
sem_t *capacity;
sem_t *pass_boarded;

void cleanup_semaphores() {
  sem_close(ready);
  sem_close(boats);
  sem_close(capacity);
  sem_close(pass_boarded);

  sem_unlink(SEM_READY);
  sem_unlink(SEM_BOATS);
  sem_unlink(SEM_CAPACITY);
  sem_unlink(SEM_PASS_BOARDED);
}

void passenger_process(int id) {
    sem_wait(ready);
    printf("Passenger %d boarded\n", id);
    sem_post(pass_boarded);
    exit(0);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
  pid_t pid;
  init_semaphores(n_capacity, n_boats);

  for (int i = 0; i < n_passengers; i++) {
    pid = fork();
    if (pid == 0) {
      passenger_process(i);
    }
  }

  if (pid != 0) {
    // Parent process: Manage lifeboats and passengers
    while (passengers_boarded < n_passengers) 
    {
        for (int boat_index = 0; boat_index < n_boats; boat_index++) 
        {  
            for (int capacity_index = 0; capacity_index < n_capacity; capacity_index++) 
            {
                int val;
                printf("Boat %d boarded passenger\n", boat_index);
                sem_post(ready);  
                sem_getvalue(ready, &val);
                printf("%d\n", val);
                sem_wait(pass_boarded); // Wait for the passenger to board
                passengers_boarded++;
            }
        printf("Boat %d finished boarding and is leaving\n", boat_index);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < n_passengers; i++) {
      wait(NULL);
    }

    cleanup_semaphores();
    printf("All passengers have boarded lifeboats.\n");
    return 0;
  }
}
