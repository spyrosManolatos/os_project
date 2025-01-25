#include <fcntl.h> // For O_CREAT and O_EXCL
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ipc_utils.h"

int current_lifeboat = 2;
int passengers_boarded = 0;
int n_passengers = 30;
int n_boats = 5;
int n_capacity = 4;

sem_t *ready;
sem_t *boats;
sem_t *capacity;
sem_t *pass_boarded;

// void init_semaphores(int n_capacity, int n_lboats)
// {
//   ready = sem_open(SEM_READY, O_CREAT, 0644, 0);
//   boats = sem_open(SEM_BOATS, O_CREAT, 0644, n_lboats);
//   capacity = sem_open(SEM_CAPACITY, O_CREAT, 0644, n_capacity);
//   pass_boarded = sem_open(SEM_PASS_BOARDED, O_CREAT, 0644, 0);
// }

// void destroy_semaphores()
// {
//   sem_close(ready);
//   sem_close(boats);
//   sem_close(capacity);
//   sem_close(pass_boarded);

//   sem_unlink(SEM_READY);
//   sem_unlink(SEM_BOATS);
//   sem_unlink(SEM_CAPACITY);
//   sem_unlink(SEM_PASS_BOARDED);
// }
int main()
{
  setvbuf(stdout, NULL, _IONBF, 0);
  pid_t pid;
  init_semaphores(n_capacity, n_boats);

  for (int i = 0; i < n_passengers; i++)
  {
    pid = fork();
    if (pid == 0)
    {
      passenger_process(i);
    }
  }

  if (pid != 0)
  {
    while (passengers_boarded < n_passengers)
    {
      for (int boat = 0; boat < n_boats && passengers_boarded < n_passengers; boat++)
      {
        int boat_passengers = 0;
        printf("\nBoat %d is loading passengers\n", boat);

        // Load up to capacity or remaining passengers
        while (boat_passengers < n_capacity && passengers_boarded < n_passengers)
        {
          sem_post(ready);
          sem_wait(pass_boarded);
          boat_passengers++;
          passengers_boarded++;
          printf("Boat %d has %d/%d passengers\n", boat, boat_passengers, n_capacity);
        }

        printf("Boat %d departing with %d passengers\n", boat, boat_passengers);
      }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < n_passengers; i++)
    {
      wait(NULL);
    }

    destroy_semaphores();
    printf("All passengers have boarded lifeboats.\n");
    return 0;
  }
}
