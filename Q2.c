#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef struct {
    char name[256];
    int priority;
    pid_t pid;
    int address;
    int memory;
    int runtime;
    int suspended;
} proc;

typedef struct {
    proc* items[1024];
    int front;
    int rear;
    int size;
} FIFOQueue;

void initializeQueue(FIFOQueue* queue) {
    queue->front = 0;
    queue->rear = 1024 - 1;
    queue->size = 0;
}

int isFull(FIFOQueue* queue) {
    return (queue->size == 1024);
}

int isEmpty(FIFOQueue* queue) {
    return (queue->size == 0);
}

void push(FIFOQueue* queue, proc* item) {
    if (isFull(queue)) {
        printf("Queue is full.\n");
        return;
    }
    queue->rear = (queue->rear + 1) % 1024;
    queue->items[queue->rear] = item;
    queue->size++;
}

proc* pop(FIFOQueue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return NULL;
    }
    proc* item = queue->items[queue->front];
    queue->front = (queue->front + 1) % 1024;
    queue->size--;
    return item;
}

void executeProcess(proc* process) {
    printf("Executing process: %s \nPriority:%d     PID:%d     Memory:%d     Runtime:%dseconds\n", 
           process->name, process->priority, process->pid, process->memory, process->runtime);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl("./sigtrap", "sigtrap", NULL);
        exit(0);
    } else if (pid > 0) {
        // Parent process
        process->pid = pid;
        sleep(process->runtime);
        kill(pid, SIGTSTP);
        waitpid(pid, NULL, 0);
        process->address = 0; // Reset memory allocation
        printf("Process %s finished execution.\n\n", process->name);
    } else {
        printf("Fork failed.\n");
    }
}

int main() {
    FILE* file = fopen("processes_q2.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    proc* processes[1024];
    FIFOQueue priorityQueue, secondaryQueue;
    initializeQueue(&priorityQueue);
    initializeQueue(&secondaryQueue);

    char line[512];
    while (fgets(line, sizeof(line), file) != NULL) {
        proc* process = malloc(sizeof(proc));
        if (process == NULL) {
            printf("Memory allocation failed.\n");
            return 1;
        }
        sscanf(line, "%[^,], %d, %d, %d\n", process->name, &process->priority, &process->memory, &process->runtime);
        process->pid = 0;
        process->address = 0;
        process->suspended = 0;

        if (process->priority == 0) {
            push(&priorityQueue, process);
        } else {
            push(&secondaryQueue, process);
        }
    }
    fclose(file);

    // Execute processes in priority queue
    while (!isEmpty(&priorityQueue)) {
        proc* process = pop(&priorityQueue);
        executeProcess(process);
    }

    // Execute processes in secondary queue
    while (!isEmpty(&secondaryQueue)) {
        proc* process = pop(&secondaryQueue);
        if (process->memory <= 1024) {
            executeProcess(process);
        } else {
            printf("not enough memory for process %s. Pushing it back to the queue.\n", process->name);
            push(&secondaryQueue, process);
        }
    }

    // Free allocated memory
    for (int i = 0; i < 1024; i++) {
        if (processes[i] != NULL) {
            free(processes[i]);
        }
    }

    return 0;
}

