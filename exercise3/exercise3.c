#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MEMORY_SIZE 512       // Total memory size (KB)
#define TIME_QUANTUM 3        // Quantum (msec)
#define MAX_NUM_PROCESSES 100 // Max number of processes

// Process structure
typedef struct {
    int pid;
    int arrival_time;
    int duration;
    int remaining_time;
    int memory_needed;
    bool in_memory;
} Process;

// Memory block structure
typedef struct {
    int start;
    int size;
    bool free;
    int pid; // -1 if free
} MemoryBlock;

// Global variables
MemoryBlock memory[MEMORY_SIZE];      // Memory represented as an array of struct
Process processes[MAX_NUM_PROCESSES]; // Processes in an array of struct
int process_count = 0;                // Number of process

// Memory initialization
void initialize_memory() {
    memory[0].start = 0;
    memory[0].size = MEMORY_SIZE;
    memory[0].free = true;
    memory[0].pid = -1;
}
