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

// Struct arrays and Global variables
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

// Find free block of memory (First Fit)
int allocate_memory(int pid, int size) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].free && memory[i].size >= size) {
            memory[i].free = false;
            memory[i].pid = pid;

            if (memory[i].size > size) {
                // Block separation
                for (int j = MEMORY_SIZE - 1; j > i; j--) {
                    memory[j] = memory[j - 1];
                }
                memory[i + 1].start = memory[i].start + size;
                memory[i + 1].size = memory[i].size - size;
                memory[i + 1].free = true;
                memory[i + 1].pid = -1;
                memory[i].size = size;
            }
            return memory[i].start;
        }
    }
    return -1; // Not enougn space
}

// Freeing memory
void free_memory(int pid) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].pid == pid) {
            memory[i].free = true;
            memory[i].pid = -1;

            // Merge with neighboring blocks
            if (i > 0 && memory[i - 1].free) {
                memory[i - 1].size += memory[i].size;
                for (int j = i; j < MEMORY_SIZE - 1; j++) {
                    memory[j] = memory[j + 1];
                }
                i--;
            }
            if (i < MEMORY_SIZE - 1 && memory[i + 1].free) {
                memory[i].size += memory[i + 1].size;
                for (int j = i + 1; j < MEMORY_SIZE - 1; j++) {
                    memory[j] = memory[j + 1];
                }
            }
        }
    }
}

// Print memory status
void print_memory() {
    printf("Memory Status:\n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].size > 0) {
            printf("Block Start: %d, Size: %d, Free: %s, PID: %d\n",
                   memory[i].start, memory[i].size,
                   memory[i].free ? "Yes" : "No", memory[i].pid);
        }
    }
}

// Round Robin simulation
void simulate_round_robin() {

    
}

// Main
int main(){

    
}
