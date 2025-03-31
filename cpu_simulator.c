#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Constants for simulation
#define NUM_REGISTERS 8
#define STACK_SIZE 256
#define HEAP_SIZE 1024
#define MAX_TASKS 5
#define TIME_SLICE 2 // Time slice in seconds

// CPU structure
typedef struct {
    int registers[NUM_REGISTERS]; // General-purpose registers
    int pc; // Program Counter
    int sp; // Stack Pointer
    char stack[STACK_SIZE];
    char heap[HEAP_SIZE];
} CPU;

// Task structure
typedef struct {
    int id;                 // Task ID
    int priority;           // Task priority (higher value = higher priority)
    CPU cpu_state;          // Task's CPU state
    int is_complete;        // Task completion status
    const char **program;   // Task-specific program
    pthread_t thread;       // Thread associated with the task
    int preempt_flag;       // Flag to signal preemption
} Task;

// Global task queue and task count
Task task_queue[MAX_TASKS];
int task_count = 0;

// Mutex for thread synchronization
pthread_mutex_t scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
void add_task(int priority, const char *program[]);
void *execute_task(void *arg);
void scheduler();
void execute_instruction(CPU *cpu, const char *instruction, int *pc);
void initialize_cpu(CPU *cpu);

int main() {
    // Example programs for tasks
    const char *program1[] = {
        "LOAD R0, 10",
        "LOAD R1, 20",
        "ADD R2, R0, R1",
        "PUSH R2",
        "POP R3",
        "HALT",
        NULL
    };

    const char *program2[] = {
        "LOAD R0, 50",
        "LOAD R1, 5",
        "SUB R2, R0, R1",
        "HALT",
        NULL
    };

    const char *program3[] = {
        "LOAD R0, 100",
        "LOAD R1, 4",
        "MUL R2, R0, R1",
        "HALT",
        NULL
    };

    // Add tasks
    add_task(3, program1); // High-priority task
    add_task(1, program2); // Low-priority task
    add_task(2, program3); // Medium-priority task

    // Run scheduler
    scheduler();

    return 0;
}

// Function to add a task to the queue
void add_task(int priority, const char *program[]) {
    if (task_count >= MAX_TASKS) {
        printf("Task queue full!\n");
        return;
    }

    Task new_task;
    new_task.id = task_count;
    new_task.priority = priority;
    new_task.is_complete = 0;
    new_task.program = program;
    new_task.preempt_flag = 0;
    initialize_cpu(&new_task.cpu_state);

    task_queue[task_count++] = new_task;
    printf("Task %d added with priority %d\n", new_task.id, priority);
}

// Initialize the CPU state for a task
void initialize_cpu(CPU *cpu) {
    cpu->pc = 0;
    cpu->sp = STACK_SIZE - 1;
    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->stack, 0, sizeof(cpu->stack));
    memset(cpu->heap, 0, sizeof(cpu->heap));
}

// Task scheduler (round-robin with signaling)
void scheduler() {
    printf("\nStarting task scheduler...\n");

    while (1) {
        pthread_mutex_lock(&scheduler_mutex); // Lock for synchronization

        int active_tasks = 0;

        for (int i = 0; i < task_count; i++) {
            if (!task_queue[i].is_complete) {
                active_tasks++;
                printf("\nScheduling Task %d (Priority: %d)\n", task_queue[i].id, task_queue[i].priority);

                // Reset preemption flag and create a thread for the task
                task_queue[i].preempt_flag = 0;
                pthread_create(&task_queue[i].thread, NULL, execute_task, &task_queue[i]);

                // Allow task to run for its time slice
                sleep(TIME_SLICE);

                // Signal preemption
                task_queue[i].preempt_flag = 1;
                pthread_join(task_queue[i].thread, NULL);

                if (!task_queue[i].is_complete) {
                    printf("Task %d preempted after %d seconds\n", task_queue[i].id, TIME_SLICE);
                }
            }
        }

        pthread_mutex_unlock(&scheduler_mutex); // Unlock after scheduling

        if (active_tasks == 0) {
            printf("All tasks completed!\n");
            break;
        }
    }
}

// Execute a task
void *execute_task(void *arg) {
    Task *task = (Task *)arg;
    CPU *cpu = &task->cpu_state;

    while (task->program[cpu->pc] != NULL) {
        if (task->preempt_flag) {
            printf("Task %d preempted!\n", task->id);
            break;
        }

        execute_instruction(cpu, task->program[cpu->pc], &cpu->pc);

        if (strcmp(task->program[cpu->pc], "HALT") == 0) {
            task->is_complete = 1;
            printf("Task %d completed\n", task->id);
            break;
        }
    }

    return NULL;
}

// Execute a single instruction
void execute_instruction(CPU *cpu, const char *instruction, int *pc) {
    char op[10];
    char arg1[10], arg2[10], arg3[10];

    sscanf(instruction, "%s %s %s %s", op, arg1, arg2, arg3);

    if (strcmp(op, "LOAD") == 0) {
        int reg_index = arg1[1] - '0';
        int value = atoi(arg2);
        cpu->registers[reg_index] = value;
        printf("Loaded %d into R%d\n", value, reg_index);
    } else if (strcmp(op, "ADD") == 0) {
        int reg1 = arg1[1] - '0';
        int reg2 = arg2[1] - '0';
        cpu->registers[reg1] = cpu->registers[reg1] + cpu->registers[reg2];
        printf("R%d = R%d + R%d\n", reg1, reg1, reg2);
    } else if (strcmp(op, "SUB") == 0) {
        int reg1 = arg1[1] - '0';
        int reg2 = arg2[1] - '0';
        cpu->registers[reg1] = cpu->registers[reg1] - cpu->registers[reg2];
        printf("R%d = R%d - R%d\n", reg1, reg1, reg2);
    } else if (strcmp(op, "MUL") == 0) {
        int reg1 = arg1[1] - '0';
        int reg2 = arg2[1] - '0';
        cpu->registers[reg1] = cpu->registers[reg1] * cpu->registers[reg2];
        printf("R%d = R%d * R%d\n", reg1, reg1, reg2);
    } else if (strcmp(op, "PUSH") == 0) {
        int reg_index = arg1[1] - '0';
        if (cpu->sp >= 0) {
            cpu->stack[cpu->sp--] = cpu->registers[reg_index];
            printf("Pushed R%d to stack\n", reg_index);
        } else {
            printf("Stack Overflow!\n");
        }
    } else if (strcmp(op, "POP") == 0) {
        int reg_index = arg1[1] - '0';
        if (cpu->sp < STACK_SIZE - 1) {
            cpu->registers[reg_index] = cpu->stack[++cpu->sp];
            printf("Popped to R%d\n", reg_index);
        } else {
            printf("Stack Underflow!\n");
        }
    } else if (strcmp(op, "HALT") == 0) {
        printf("Halting task execution\n");
    } else {
        printf("Unknown instruction: %s\n", instruction);
    }

    (*pc)++; // Increment the program counter
}
