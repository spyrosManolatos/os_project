///////////////////////////////////
// Manolatos Spyridon, 1104802
// Maroudas Andreas-Erikos, 1100617
// Kirkinis Manousos, 1100590
// Chrysanthakopoulos Nikolaos, 1100760
////////////////////////////////////
#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

#define SEM_READY "/sem_ready"
#define SEM_BOATS "/sem_boats"
#define SEM_CAPACITY "/sem_capacity"
#define SEM_PASS_BOARDED "/sem_pass_boarded"

// Declare external semaphores
extern sem_t *ready;
extern sem_t *boats;
extern sem_t *capacity;
extern sem_t *pass_boarded;
// a keyword that prevents multiple definition errors in header files
static inline void init_semaphores(int n_capacity, int n_lboats){
    ready = sem_open(SEM_READY, O_CREAT, 0644, 0);
    boats = sem_open(SEM_BOATS, O_CREAT, 0644, n_lboats);
    capacity = sem_open(SEM_CAPACITY, O_CREAT, 0644, n_capacity);
    pass_boarded = sem_open(SEM_PASS_BOARDED, O_CREAT, 0644, 0);
}
void passenger_process(int id);
static inline void destroy_semaphores(){
    sem_close(ready);
    sem_close(boats);
    sem_close(capacity);
    sem_close(pass_boarded);
}

#endif