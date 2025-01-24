#include <semaphore.h>
#include <pthread.h>

sem_t boat_semaphore;
sem_t boarding_queue_semaphore;

void init_semaphores(int n_capacity, int n_lboats) {
  ready = sem_open(SEM_READY, O_CREAT, 0644, 0);
  boats = sem_open(SEM_BOATS, O_CREAT, 0644, n_lboats);
  capacity = sem_open(SEM_CAPACITY, O_CREAT, 0644, n_capacity);
  pass_boarded = sem_open(SEM_PASS_BOARDED, O_CREAT, 0644, 0);
}

void destroy_semaphores() {
  sem_close(ready);
  sem_close(boats);
  sem_close(capacity);
  sem_close(pass_boarded);

  sem_unlink(SEM_READY);
  sem_unlink(SEM_BOATS);
  sem_unlink(SEM_CAPACITY);
  sem_unlink(SEM_PASS_BOARDED);
}