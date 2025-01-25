//////////////////////////////////////////
// Manolatos Spyridon, 1104802
// Maroudas Andreas-Erikos, 1100617
// Kirkinis Manousos, 1100590
// Chrysanthakopoulos Nikolaos, 1100760
///////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MEMORY_SIZE 512
#define TIME_QUANTUM 3
#define MAX_PROCESSES 5

typedef struct {
    int pid;
    int arrival_time;
    int duration;
    int remaining_time;
    int memory_needed;
    bool in_memory;
} Process;

typedef struct {
    int start;
    int size;
    bool free;
    int pid;
} MemoryBlock;

MemoryBlock memory[MEMORY_SIZE];
Process processes[MAX_PROCESSES];
int process_count = 0;
int current_time = 0;

void initialize_memory() {
    memory[0].start = 0;
    memory[0].size = MEMORY_SIZE;
    memory[0].free = true;
    memory[0].pid = -1;
    for (int i = 1; i < MEMORY_SIZE; i++) {
        memory[i].size = 0;
    }
}

int find_memory(int memory_needed) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].free && memory[i].size >= memory_needed) {
            return i;
        }
    }
    return -1;
}

void allocate_memory(int process_index, int block_index) {
    int memory_needed = processes[process_index].memory_needed;
    MemoryBlock *block = &memory[block_index];
    block->free = false;
    block->pid = processes[process_index].pid;

    if (block->size > memory_needed) {
        MemoryBlock new_block = {
            .start = block->start + memory_needed,
            .size = block->size - memory_needed,
            .free = true,
            .pid = -1
        };
        for (int j = MEMORY_SIZE - 1; j > block_index; j--) {
            memory[j] = memory[j - 1];
        }
        memory[block_index + 1] = new_block;
    }

    block->size = memory_needed;
    processes[process_index].in_memory = true;
}

void free_memory(int pid) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].pid == pid) {
            memory[i].free = true;
            memory[i].pid = -1;

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

void release_all_memory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i].free = true;
        memory[i].pid = -1;
        if (i > 0) {
            memory[i].size = 0;
        }
    }
    memory[0].size = MEMORY_SIZE;
    process_count = 0;
    printf("All memory resources have been deallocated.\n");
}

void round_robin() {
    int completed = 0;
    while (completed < process_count) {
        bool progress_made = false;

        for (int i = 0; i < process_count; i++) {
            if (processes[i].remaining_time <= 0) {
                continue;
            }
            if(processes[i].memory_needed > MEMORY_SIZE){
                printf("Process %d requires more memory than available. Process not executed.\n", processes[i].pid);
                exit(1);
            }
            if (processes[i].arrival_time <= current_time) {
                if (!processes[i].in_memory) {
                    int block_index = find_memory(processes[i].memory_needed);
                    if (block_index != -1) {
                        allocate_memory(i, block_index);
                        printf("Process %d loaded into memory.\n", processes[i].pid);
                    } else {
                        continue;
                    }
                }

                int execution_time = (processes[i].remaining_time < TIME_QUANTUM) ? processes[i].remaining_time : TIME_QUANTUM;
                processes[i].remaining_time -= execution_time;
                current_time += execution_time;
                progress_made = true;

                printf("Time %d: Process %d executed for %dms, remaining time: %dms\n",
                       current_time, processes[i].pid, execution_time, processes[i].remaining_time);

                if (processes[i].remaining_time == 0) {
                    completed++;
                    free_memory(processes[i].pid);
                    printf("Process %d completed and memory freed.\n", processes[i].pid);
                }
            }
        }

        if (!progress_made) {
            current_time++;
        }
    }
    release_all_memory();
}

void add_process() {
    if (process_count >= MAX_PROCESSES) {
        printf("Maximum number of processes reached.\n");
        return;
    }

    Process new_process;
    new_process.pid = process_count + 1;
    printf("Enter arrival time for process %d: ", new_process.pid);
    scanf("%d", &new_process.arrival_time);
    printf("Enter duration for process %d: ", new_process.pid);
    scanf("%d", &new_process.duration);

    if (new_process.duration <= 0) {
        printf("Invalid duration. Process not added.\n");
        return;
    }

    new_process.remaining_time = new_process.duration;
    printf("Enter memory needed for process %d (in KB): ", new_process.pid);
    scanf("%d", &new_process.memory_needed);
    new_process.in_memory = false;

    processes[process_count++] = new_process;
}

int main() {
    initialize_memory();

    int choice;
    do {
        printf("1. Add Process\n2. Run Round Robin\n3. Exit\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_process();
                break;
            case 2:
                round_robin();
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 3);

    return 0;
}