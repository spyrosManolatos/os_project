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
    int assigned_cores[MAX_CORES];
    double t_submission, t_start, t_end;
} proc_t;

int assign_cores(proc_t *proc);

struct single_queue {
    proc_t    *first;
    proc_t    *last;
    long members;
};

struct single_queue global_q;

#define proc_queue_empty(q) ((q)->first==NULL)

void proc_queue_init (register struct single_queue * q)
{
    q->first = q->last = NULL;
    q->members = 0;
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
        printf("proc: [name:%s pid:%d requested_cores:%d assigned_cores:", 
            proc->name, proc->pid, proc->requested_cores);
        for(int i = 0; i < proc->requested_cores; i++) {
            if (proc->assigned_cores[i] != -1) {
                printf("%d ", proc->assigned_cores[i]);
            }
        }
        printf("]\n");
        proc = proc->next;
    }
}

double proc_gettime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double) (tv.tv_sec+tv.tv_usec/1000000.0);
}
#define RR        1
#define FCFS    2
#define RRFF   3
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
        case FCFS:
            fcfs(core_id);
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

    if (!strcmp(argv[2], "FCFS")) {
        policy = FCFS;
        input = fopen(argv[3], "r");
        printf("Policy: FCFS\n");
        if (input == NULL) err_exit("invalid input file name");
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
        proc->requested_cores = token ? atoi(token) : 1; // Διαβάζουμε τον αριθμό των πυρήνων
        proc->pid = -1;
        proc->status = PROC_NEW;
        proc->t_submission = proc_gettime();
        for (int i = 0; i < MAX_CORES; i++) {
            proc->assigned_cores[i] = -1; // Αρχικοποίηση των ανατεθέντων πυρήνων
        }
        proc_to_rq_end(proc);
    }

    global_t = proc_gettime();

    for (int i = 0; i < num_cores; i++) {
        core_ids[i] = i;
        pthread_create(&cores[i], NULL, core_scheduler, &core_ids[i]);
    }

    for (int i = 0; i < num_cores; i++) {
        pthread_join(cores[i], NULL);
    }

    print_queue(); // Εκτύπωση της ουράς μετά την ανάθεση των πυρήνων

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

void fcfs(int core_id)
{
    struct sigaction sig_act;
    proc_t *proc;
    int pid;

    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_handler = 0;
    sig_act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    sig_act.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD, &sig_act, NULL);

    while ((proc = proc_rq_dequeue())!=NULL) {

        printf("Core %d: Dequeue process with name %s and pid %d\n", core_id, proc->name, proc->pid);

        // Ανάθεση πυρήνων στη διεργασία
        if (!assign_cores(proc)) {
            printf("Core %d: Not enough cores available for process %s\n", core_id, proc->name);
            printf("I am going to the end of the queue till the cores are available\n");
            proc->status = PROC_STOPPED;
            proc_to_rq_end(proc);
            print_queue();
            sleep(1);
            continue;
        }

        // Εκτύπωση των ανατεθέντων πυρήνων
        printf("Process %s assigned to cores: ", proc->name);
        for (int i = 0; i < proc->requested_cores; i++) {
            printf("%d ", proc->assigned_cores[i]);
        }
        printf("\n");
        proc->status = PROC_NEW;

        // Εκτύπωση της ουράς μετά την ανάθεση των πυρήνων
        print_queue();

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
                proc->status = PROC_RUNNING;

                // Αναμονή μέχρι να ολοκληρωθεί η διεργασία
                while (proc->status == PROC_RUNNING) {
                    pause();
                }

                // Απελευθέρωση των πυρήνων μετά την ολοκλήρωση της διεργασίας
                for (int i = 0; i < proc->requested_cores; i++) {
                    running_proc[proc->assigned_cores[i]] = NULL;
                }
            }
        } else if (proc->status == PROC_EXITED) {
            printf("Core %d: process has exited\n", core_id);
        } else {
            err_exit("Unknown process status");
        }
    }
}

int assign_cores(proc_t *proc)
{
    int allocated_cores = 0;
    int core_indices[MAX_CORES];

    printf("Assigning cores for process %s requesting %d cores\n", proc->name, proc->requested_cores);

    for (int i = 0; i < num_cores; i++) {
        if (running_proc[i] == NULL) {
            core_indices[allocated_cores] = i;
            allocated_cores++;
            if (allocated_cores == proc->requested_cores) {
                break;
            }
        }
    }

    if (allocated_cores < proc->requested_cores) {
        printf("Not enough cores available for process %s\n", proc->name);
        return 0; // Δεν υπάρχουν αρκετοί διαθέσιμοι πυρήνες
    }

    // Ανάθεση των πυρήνων στη διεργασία
    for (int i = 0; i < proc->requested_cores; i++) {
        proc->assigned_cores[i] = core_indices[i];
        running_proc[core_indices[i]] = proc;
        printf("Assigned core %d to process %s\n", core_indices[i], proc->name);
    }

    return 1; // Επιτυχής ανάθεση
}
