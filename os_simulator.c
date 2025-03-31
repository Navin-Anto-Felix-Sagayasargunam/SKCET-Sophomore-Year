#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Constants
#define MAX_PROCESSES 5
#define MEMORY_SIZE 1024
#define PAGE_SIZE 256
#define MAX_FILES 10
#define TIME_SLICE 2
#define MAX_DEVICE_QUEUE 5

// Process States
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// Process Structure
typedef struct {
    int id;
    char name[50];
    ProcessState state;
    int priority;
    int burst_time;
    pthread_t thread;
} Process;

// File Structure
typedef struct {
    char name[50];
    int size;
    int permissions; // Bitmask: Read (1), Write (2), Execute (4)
} File;

// Memory Management Structure
typedef struct {
    int pages[MEMORY_SIZE / PAGE_SIZE];
    int page_usage;
} Memory;

// I/O Device Structure
typedef struct {
    char name[50];
    char queue[MAX_DEVICE_QUEUE][50];
    int queue_count;
    sem_t semaphore; // Synchronization for device queue
} Device;

// Global Tables
Process process_table[MAX_PROCESSES];
File file_table[MAX_FILES];
Memory memory_manager;
Device device_manager;

int process_count = 0;
int file_count = 0;

// Mutex for Process Synchronization
pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function Prototypes
void create_process(const char *name, int priority, int burst_time);
void schedule_processes();
void *execute_process(void *arg);
void allocate_memory(int process_id);
void deallocate_memory(int process_id);
void create_file(const char *name, int size, int permissions);
void list_files();
void perform_io_operation(const char *device_name, const char *operation);

int main() {
    // Initialize Memory Manager
    memset(memory_manager.pages, -1, sizeof(memory_manager.pages));
    memory_manager.page_usage = 0;

    // Initialize Device Manager
    strcpy(device_manager.name, "Disk");
    device_manager.queue_count = 0;
    sem_init(&device_manager.semaphore, 0, 1);

    // Create Processes
    create_process("Process1", 3, 6); // High-priority
    create_process("Process2", 2, 4); // Medium-priority
    create_process("Process3", 1, 5); // Low-priority

    // Allocate Memory to Processes
    allocate_memory(0);
    allocate_memory(1);
    allocate_memory(2);

    // Schedule Processes
    schedule_processes();

    // Deallocate Memory
    deallocate_memory(0);
    deallocate_memory(1);
    deallocate_memory(2);

    // Create Files
    create_file("file1.txt", 256, 1 | 2); // Read and Write
    create_file("file2.txt", 128, 1 | 4); // Read and Execute
    list_files();

    // Perform I/O Operations
    perform_io_operation("Disk", "Read");
    perform_io_operation("Disk", "Write");

    return 0;
}

// Create a Process
void create_process(const char *name, int priority, int burst_time) {
    if (process_count >= MAX_PROCESSES) {
        printf("Maximum process limit reached!\n");
        return;
    }
    Process *p = &process_table[process_count];
    p->id = process_count;
    strcpy(p->name, name);
    p->state = NEW;
    p->priority = priority;
    p->burst_time = burst_time;
    process_count++;
    printf("Process %s created with priority %d\n", name, priority);
}

// Allocate Memory
void allocate_memory(int process_id) {
    if (memory_manager.page_usage >= MEMORY_SIZE / PAGE_SIZE) {
        printf("Memory allocation failed for Process %d\n", process_id);
        return;
    }
    memory_manager.pages[memory_manager.page_usage++] = process_id;
    printf("Allocated memory page to Process %d\n", process_id);
}

// Deallocate Memory
void deallocate_memory(int process_id) {
    for (int i = 0; i < memory_manager.page_usage; i++) {
        if (memory_manager.pages[i] == process_id) {
            memory_manager.pages[i] = -1;
            printf("Deallocated memory page of Process %d\n", process_id);
        }
    }
}

// Schedule Processes using Round-Robin
void schedule_processes() {
    printf("\nScheduling Processes...\n");
    while (1) {
        pthread_mutex_lock(&process_mutex);
        int active_processes = 0;
        for (int i = 0; i < process_count; i++) {
            Process *p = &process_table[i];
            if (p->state != TERMINATED) {
                active_processes++;
                p->state = RUNNING;
                printf("Running Process %s\n", p->name);
                sleep(TIME_SLICE);
                p->burst_time -= TIME_SLICE;
                if (p->burst_time <= 0) {
                    p->state = TERMINATED;
                    printf("Process %s terminated\n", p->name);
                } else {
                    p->state = READY;
                }
            }
        }
        pthread_mutex_unlock(&process_mutex);
        if (active_processes == 0) break;
    }
}

// Create a File
void create_file(const char *name, int size, int permissions) {
    if (file_count >= MAX_FILES) {
        printf("Maximum file limit reached!\n");
        return;
    }
    File *f = &file_table[file_count++];
    strcpy(f->name, name);
    f->size = size;
    f->permissions = permissions;
    printf("File %s created with size %d bytes and permissions %d\n", name, size, permissions);
}

// List Files
void list_files() {
    printf("\nListing Files...\n");
    for (int i = 0; i < file_count; i++) {
        printf("File: %s, Size: %d bytes, Permissions: %d\n", file_table[i].name, file_table[i].size, file_table[i].permissions);
    }
}

// Perform I/O Operation
void perform_io_operation(const char *device_name, const char *operation) {
    if (strcmp(device_name, device_manager.name) != 0) {
        printf("Unknown device: %s\n", device_name);
        return;
    }
    if (device_manager.queue_count >= MAX_DEVICE_QUEUE) {
        printf("Device queue full for %s\n", device_name);
        return;
    }
    strcpy(device_manager.queue[device_manager.queue_count++], operation);
    sem_wait(&device_manager.semaphore);
    printf("Performing %s operation on %s\n", operation, device_name);
    device_manager.queue_count--;
    sem_post(&device_manager.semaphore);
}
