#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>

#define MAX_LINE_LENGTH 80
#define MAX_CORES 4

void rr(int core_id);
void rr_aff(int core_id);
void fcfs(int core_id);

#define PROC_NEW    0
#define PROC_STOPPED    1
#define PROC_RUNNING    2
#define PROC_EXITED    3

typedef struct proc_desc {
    struct proc_desc *next;
    char name[80];
    int pid;
    int status;
    int requested_cores;
    int first_core;
    double t_submission, t_start, t_end;
} proc_t;

struct single_queue {
    proc_t    *first;
    proc_t    *last;
    long members;
};

struct single_queue global_q;
struct single_queue local_q[MAX_CORES];

#define proc_queue_empty(q) ((q)->first==NULL)

void proc_queue_init (register struct single_queue * q)
{
    q->first = q->last = NULL;
    q->members = 0;
}

void core_queues_init()
{
    for (int i = 0; i < MAX_CORES; i++) {
        local_q[i].first = NULL;
        local_q[i].last = NULL;
    }
}

void proc_to_rq (register proc_t *proc)
{
    if (proc_queue_empty (&global_q))
        global_q.last = proc;
    proc->next = global_q.first;
    global_q.first = proc;
}

void proc_to_rq_end (register proc_t *proc)
{
    if (proc_queue_empty (&global_q))
        global_q.first = global_q.last = proc;
    else {
        global_q.last->next = proc;
        global_q.last = proc;
        proc->next = NULL;
    }
}

proc_t *proc_rq_dequeue ()
{
    register proc_t *proc;

    proc = global_q.first;
    if (proc==NULL) return NULL;

    proc = global_q.first;
    if (proc!=NULL) {
        global_q.first = proc->next;
        proc->next = NULL;
    }

    return proc;
}

void print_queue()
{
    proc_t *proc;

    proc = global_q.first;
    while (proc != NULL) {
        printf("proc: [name:%s pid:%d requested_cores:%d]\n", 
            proc->name, proc->pid, proc->requested_cores);
        proc = proc->next;
    }
}

double proc_gettime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double) (tv.tv_sec+tv.tv_usec/1000000.0);
}

#define RR      1
#define FCFS    2
#define RRAFF  3

int policy;
int quantum = 100;    /* ms */
proc_t *running_proc[MAX_CORES];
double global_t;
int num_cores;

void err_exit(char *msg)
{
    printf("Error: %s\n", msg);
    exit(1);
}

void *core_scheduler(void *arg)
{
    int core_id = *(int *)arg;
    switch(policy) {
        case RR:
            rr(core_id);
            break;
        case FCFS:
            fcfs(core_id);
            break;
        case RRAFF:
            rr_aff(core_id);
            break;
        default:
            err_exit("Unimplemented policy");
            break;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    FILE *input;
    char exec[80];
    proc_t *proc;
    pthread_t cores[MAX_CORES];
    int core_ids[MAX_CORES];
    printf("argc = %d\n", argc);
    if (argc < 4) {
        err_exit("invalid usage");
    }

    num_cores = atoi(argv[1]);
    if (num_cores > MAX_CORES) {
        err_exit("number of cores exceeds maximum limit");
    }

    if (!strcmp(argv[2], "RR")) {
        policy = RR;
        quantum = atoi(argv[3]);
        input = fopen(argv[4], "r");
        printf("Policy: RR with quantum:%d\n", quantum);
        if (input == NULL) err_exit("invalid input file name");
    } else if (!strcmp(argv[2], "FCFS")) {
        policy = FCFS;
        input = fopen(argv[3], "r");
        printf("Policy: FCFS\n");
        if (input == NULL) err_exit("invalid input file name");
    } else if(!strcmp(argv[2], "RRAFF")) {
        policy = RRAFF;
        quantum = atoi(argv[3]);
        input = fopen(argv[4], "r");
        printf("Policy: RRAFF with quantum:%d\n", quantum);
        if (input == NULL) err_exit("invalid input file name");
    }
    else {
        err_exit("invalid usage");
    }

    /* Read input file */
    while (fgets(exec, sizeof(exec), input) != NULL) {
        // Remove newline character if present
        exec[strcspn(exec, "\n")] = 0;
        if (strlen(exec) == 0) {
            continue;
        }
        printf("Read process %s\n", exec);
        proc = malloc(sizeof(proc_t));
        proc->next = NULL;
        char *token = strtok(exec, " ");
        strcpy(proc->name, token);
        token = strtok(NULL, " ");
        proc->requested_cores = token ? atoi(token) : 1;
        proc->pid = -1;
        proc->status = PROC_NEW;
        proc->t_submission = proc_gettime();
        proc_to_rq_end(proc);
    }

    print_queue();

    global_t = proc_gettime();

    for (int i = 0; i < num_cores; i++) {
        core_ids[i] = i;
        pthread_create(&cores[i], NULL, core_scheduler, &core_ids[i]);
    }

    for (int i = 0; i < num_cores; i++) {
        pthread_join(cores[i], NULL);
    }

    printf("WORKLOAD TIME: %.2lf secs\n", proc_gettime() - global_t);
    printf("scheduler exits\n");
    return 0;
}

void sigchld_handler(int signo, siginfo_t *info, void *context)
{
    printf("child %d exited\n", info->si_pid);
    for (int i = 0; i < num_cores; i++) {
        if (running_proc[i] != NULL && running_proc[i]->pid == info->si_pid) {
            running_proc[i]->status = PROC_EXITED;
            proc_t *proc = running_proc[i];
            proc->t_end = proc_gettime();
            printf("PID %d - CMD: %s\n", proc->pid, proc->name);
            printf("\tElapsed time = %.2lf secs\n", proc->t_end-proc->t_submission);
            printf("\tExecution time = %.2lf secs\n", proc->t_end-proc->t_start);
            printf("\tWorkload time = %.2lf secs\n", proc->t_end-global_t);
            running_proc[i] = NULL;
            break;
        }
    }
}

void rr(int core_id)
{
    printf("Quantum: %d\n", quantum);
    struct sigaction sig_act;
    proc_t *proc;
    int pid;
    struct timespec req, rem;

    req.tv_sec = quantum / 1000;
    req.tv_nsec = (quantum % 1000) * 1000000;

    printf("Core %d: tv_sec = %ld\n", core_id, req.tv_sec);
    printf("Core %d: tv_nsec = %ld\n", core_id, req.tv_nsec);

    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_handler = 0;
    sig_act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    sig_act.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD, &sig_act, NULL);

    while ((proc = proc_rq_dequeue()) != NULL) {
        printf("Core %d: Dequeue process with name %s and pid %d\n", core_id, proc->name, proc->pid);
        if (proc->status == PROC_NEW) {
            proc->t_start = proc_gettime();
            pid = fork();
            if (pid == -1) {
                err_exit("fork failed!");
            }
            if (pid == 0) {
                printf("Core %d: executing %s\n", core_id, proc->name);
                execl(proc->name, proc->name, NULL);
            } else {
                proc->pid = pid;
                running_proc[core_id] = proc;
                proc->status = PROC_RUNNING;

                nanosleep(&req, &rem);
                if (proc->status == PROC_RUNNING) {
                    kill(proc->pid, SIGSTOP);
                    proc->status = PROC_STOPPED;
                    proc_to_rq_end(proc);
                }
            }
        } else if (proc->status == PROC_STOPPED) {
            proc->status = PROC_RUNNING;
            running_proc[core_id] = proc;
            kill(proc->pid, SIGCONT);

            nanosleep(&req, &rem);
            if (proc->status == PROC_RUNNING) {
                kill(proc->pid, SIGSTOP);
                proc_to_rq_end(proc);
                proc->status = PROC_STOPPED;
            }
        } else if (proc->status == PROC_EXITED) {
            printf("Core %d: process has exited\n", core_id);
        } else if (proc->status == PROC_RUNNING) {
            printf("Core %d: WARNING: Already running process\n", core_id);
        } else {
            err_exit("Unknown process status");
        }
    }
}

void rr_aff(int core_id) {
    struct sigaction sig_act;
    proc_t *proc;
    int pid;
    struct timespec req, rem;

    // Setup quantum time
    req.tv_sec = quantum / 1000;
    req.tv_nsec = (quantum % 1000) * 1000000;

    printf("Core %d: RR-AFF scheduler starting (quantum = %d ms)\n", core_id, quantum);

    // Setup signal handler
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_handler = 0;
    sig_act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    sig_act.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD, &sig_act, NULL);

    while (1) {
        // Check if the local queue is empty and fetch a process from the global queue if available
        if (local_q[core_id].first == NULL) {
            proc = proc_rq_dequeue();
            if (proc != NULL) {
                // Add to local queue
                if (local_q[core_id].last == NULL) {
                    local_q[core_id].first = local_q[core_id].last = proc;
                } else {
                    local_q[core_id].last->next = proc;
                    local_q[core_id].last = proc;
                }
                proc->next = NULL;
            } else {
                // If all queues are empty, exit the scheduler
                int all_empty = 1;
                for (int i = 0; i < MAX_CORES; i++) {
                    if (local_q[i].first != NULL) {
                        all_empty = 0;
                        break;
                    }
                }
                if (all_empty) break;
                continue;
            }
        }

        // Fetch the first process from the local queue
        proc = local_q[core_id].first;
        if (proc == NULL) continue;

        // Remove the process from the local queue
        local_q[core_id].first = proc->next;
        if (local_q[core_id].first == NULL) {
            local_q[core_id].last = NULL;
        }

        if (proc->status == PROC_NEW) {
            proc->t_start = proc_gettime();
            proc->first_core = core_id;
            pid = fork();
            
            if (pid == -1) {
                err_exit("fork failed!");
            }
            
            if (pid == 0) {
                printf("Core %d: Executing %s\n", core_id, proc->name);
                execl(proc->name, proc->name, NULL);
                exit(1);
            }
            
            proc->pid = pid;
            proc->status = PROC_RUNNING;
        } else if (proc->status == PROC_STOPPED) {
            printf("Core %d: Resuming %s\n", core_id, proc->name);
            kill(proc->pid, SIGCONT);
            proc->status = PROC_RUNNING;
        }

        running_proc[core_id] = proc;
        nanosleep(&req, &rem);

        // Handle quantum expiration
        if (proc->status == PROC_RUNNING) {
            printf("Core %d: Stopping %s\n", core_id, proc->name);
            kill(proc->pid, SIGSTOP);
            proc->status = PROC_STOPPED;

            // Add the process back to the end of the local queue
            if (local_q[core_id].last == NULL) {
                local_q[core_id].first = local_q[core_id].last = proc;
            } else {
                local_q[core_id].last->next = proc;
                local_q[core_id].last = proc;
            }
            proc->next = NULL;
        }
    }
}


void fcfs(int core_id){
    quantum = INT_MAX;
    rr(core_id);
}
