#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 20
#define MAX_FILES 20
#define MAX_MEMORY_BLOCKS 20
#define MEMORY_SIZE 1024

/* ---------------- PROCESS MANAGEMENT ---------------- */

typedef enum {
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    int pid;
    char name[30];
    int burstTime;
    int priority;
    int memory;
    ProcessState state;
} Process;

Process processes[MAX_PROCESSES];
int processCount = 0;
int nextPID = 1001;

/* ---------------- FILE MANAGEMENT ---------------- */

typedef struct {
    char name[50];
    char content[500];
    int size;
    int exists;
} SimFile;

SimFile files[MAX_FILES];
int fileCount = 0;

/* ---------------- MEMORY MANAGEMENT ---------------- */

typedef struct {
    int processID;
    int size;
    int allocated;
} MemoryBlock;

MemoryBlock memoryBlocks[MAX_MEMORY_BLOCKS];
int memoryBlockCount = 0;
int usedMemory = 0;

/* ---------------- UTILITY FUNCTIONS ---------------- */

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
}

const char* stateName(ProcessState state) {
    switch (state) {
        case READY:
            return "READY";
        case RUNNING:
            return "RUNNING";
        case WAITING:
            return "WAITING";
        case TERMINATED:
            return "TERMINATED";
        default:
            return "UNKNOWN";
    }
}

/* ---------------- PROCESS FUNCTIONS ---------------- */

void displayProcesses() {
    int i;

    printf("\n============================================================\n");
    printf("                    PROCESS MANAGER\n");
    printf("============================================================\n");

    if (processCount == 0) {
        printf("No processes currently exist.\n");
        return;
    }

    printf("%-8s %-20s %-10s %-10s %-10s %-12s\n",
           "PID", "NAME", "BURST", "PRIORITY", "MEMORY", "STATE");

    printf("------------------------------------------------------------\n");

    for (i = 0; i < processCount; i++) {
        printf("%-8d %-20s %-10d %-10d %-10d %-12s\n",
               processes[i].pid,
               processes[i].name,
               processes[i].burstTime,
               processes[i].priority,
               processes[i].memory,
               stateName(processes[i].state));
    }
}

void createProcess() {
    if (processCount >= MAX_PROCESSES) {
        printf("\nProcess limit reached.\n");
        return;
    }

    printf("\nEnter process name: ");
    scanf("%29s", processes[processCount].name);

    printf("Enter CPU burst time: ");
    scanf("%d", &processes[processCount].burstTime);

    printf("Enter priority: ");
    scanf("%d", &processes[processCount].priority);

    printf("Enter memory required (MB): ");
    scanf("%d", &processes[processCount].memory);

    processes[processCount].pid = nextPID++;
    processes[processCount].state = READY;

    processCount++;

    printf("\nProcess created successfully!");
    printf("\nPID: %d\n", processes[processCount - 1].pid);
}

void terminateProcess() {
    int pid, i;
    int found = 0;

    printf("\nEnter PID to terminate: ");
    scanf("%d", &pid);

    for (i = 0; i < processCount; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = TERMINATED;
            found = 1;

            printf("\nProcess %d has been terminated.\n", pid);
            break;
        }
    }

    if (!found)
        printf("\nProcess not found.\n");
}

void changeProcessState() {
    int pid, choice, i;
    int found = 0;

    printf("\nEnter PID: ");
    scanf("%d", &pid);

    for (i = 0; i < processCount; i++) {
        if (processes[i].pid == pid) {

            found = 1;

            printf("\n1. READY");
            printf("\n2. RUNNING");
            printf("\n3. WAITING");
            printf("\n4. TERMINATED");

            printf("\n\nSelect state: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    processes[i].state = READY;
                    break;
                case 2:
                    processes[i].state = RUNNING;
                    break;
                case 3:
                    processes[i].state = WAITING;
                    break;
                case 4:
                    processes[i].state = TERMINATED;
                    break;
                default:
                    printf("Invalid choice.\n");
                    return;
            }

            printf("\nProcess state changed successfully.\n");
            break;
        }
    }

    if (!found)
        printf("\nProcess not found.\n");
}

/* ---------------- CPU SCHEDULING ---------------- */

void fcfsScheduling() {
    int i;
    int waiting = 0;
    int totalWaiting = 0;
    int totalTurnaround = 0;

    if (processCount == 0) {
        printf("\nNo processes available.\n");
        return;
    }

    printf("\n============================================================\n");
    printf("                FCFS CPU SCHEDULING\n");
    printf("============================================================\n");

    printf("\nGantt Chart:\n\n");

    for (i = 0; i < processCount; i++) {
        if (processes[i].state == TERMINATED)
            continue;

        printf("| P%d ", processes[i].pid);

        totalWaiting += waiting;

        waiting += processes[i].burstTime;

        totalTurnaround += waiting;
    }

    printf("|\n");

    printf("\nProcess Scheduling Information:\n");

    printf("%-10s %-12s %-15s\n",
           "PID", "Waiting Time", "Turnaround Time");

    waiting = 0;

    for (i = 0; i < processCount; i++) {

        if (processes[i].state == TERMINATED)
            continue;

        printf("%-10d %-12d %-15d\n",
               processes[i].pid,
               waiting,
               waiting + processes[i].burstTime);

        waiting += processes[i].burstTime;
    }

    printf("\nFCFS completed successfully.\n");
}

void sjfScheduling() {
    Process temp[MAX_PROCESSES];
    int n = 0;
    int i, j;
    int waiting = 0;

    for (i = 0; i < processCount; i++) {
        if (processes[i].state != TERMINATED) {
            temp[n++] = processes[i];
        }
    }

    if (n == 0) {
        printf("\nNo processes available.\n");
        return;
    }

    /* Sort according to burst time */
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (temp[j].burstTime < temp[i].burstTime) {
                Process t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }

    printf("\n============================================================\n");
    printf("                 SJF CPU SCHEDULING\n");
    printf("============================================================\n");

    printf("\nExecution Order:\n");

    for (i = 0; i < n; i++) {
        printf("P%d ", temp[i].pid);
    }

    printf("\n\n%-10s %-12s %-15s\n",
           "PID", "Waiting", "Turnaround");

    for (i = 0; i < n; i++) {

        printf("%-10d %-12d %-15d\n",
               temp[i].pid,
               waiting,
               waiting + temp[i].burstTime);

        waiting += temp[i].burstTime;
    }
}

void roundRobinScheduling() {
    int quantum;
    int remaining[MAX_PROCESSES];
    int i;
    int completed = 0;
    int time = 0;

    if (processCount == 0) {
        printf("\nNo processes available.\n");
        return;
    }

    printf("\nEnter Time Quantum: ");
    scanf("%d", &quantum);

    if (quantum <= 0) {
        printf("\nInvalid quantum.\n");
        return;
    }

    for (i = 0; i < processCount; i++) {
        remaining[i] = processes[i].burstTime;
    }

    printf("\n============================================================\n");
    printf("               ROUND ROBIN SCHEDULING\n");
    printf("============================================================\n");

    printf("\nExecution Sequence:\n");

    while (completed < processCount) {

        for (i = 0; i < processCount; i++) {

            if (remaining[i] <= 0 ||
                processes[i].state == TERMINATED)
                continue;

            printf("| P%d ", processes[i].pid);

            if (remaining[i] > quantum) {
                time += quantum;
                remaining[i] -= quantum;
            }
            else {
                time += remaining[i];
                remaining[i] = 0;
                completed++;
            }
        }
    }

    printf("|\n");
    printf("\nTotal CPU execution time: %d\n", time);
}

void schedulingMenu() {
    int choice;

    do {
        printf("\n============================================================\n");
        printf("                    CPU SCHEDULING\n");
        printf("============================================================\n");

        printf("1. FCFS\n");
        printf("2. SJF\n");
        printf("3. Round Robin\n");
        printf("4. Back\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                fcfsScheduling();
                pauseScreen();
                break;

            case 2:
                sjfScheduling();
                pauseScreen();
                break;

            case 3:
                roundRobinScheduling();
                pauseScreen();
                break;

            case 4:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 4);
}

/* ---------------- MEMORY MANAGEMENT ---------------- */

void allocateMemory() {
    int pid, size;
    int i;

    printf("\nEnter Process ID: ");
    scanf("%d", &pid);

    printf("Enter memory required (MB): ");
    scanf("%d", &size);

    if (usedMemory + size > MEMORY_SIZE) {
        printf("\nNot enough memory available.\n");
        return;
    }

    if (memoryBlockCount >= MAX_MEMORY_BLOCKS) {
        printf("\nMemory block limit reached.\n");
        return;
    }

    for (i = 0; i < processCount; i++) {
        if (processes[i].pid == pid) {

            memoryBlocks[memoryBlockCount].processID = pid;
            memoryBlocks[memoryBlockCount].size = size;
            memoryBlocks[memoryBlockCount].allocated = 1;

            memoryBlockCount++;
            usedMemory += size;

            printf("\nMemory allocated successfully.\n");
            printf("Process: P%d\n", pid);
            printf("Allocated: %d MB\n", size);

            return;
        }
    }

    printf("\nProcess not found.\n");
}

void deallocateMemory() {
    int pid;
    int i;

    printf("\nEnter Process ID: ");
    scanf("%d", &pid);

    for (i = 0; i < memoryBlockCount; i++) {

        if (memoryBlocks[i].processID == pid &&
            memoryBlocks[i].allocated == 1) {

            usedMemory -= memoryBlocks[i].size;
            memoryBlocks[i].allocated = 0;

            printf("\nMemory deallocated successfully.\n");
            return;
        }
    }

    printf("\nNo allocated memory found for this process.\n");
}

void displayMemory() {
    int i;

    printf("\n============================================================\n");
    printf("                    MEMORY MANAGER\n");
    printf("============================================================\n");

    printf("Total Memory : %d MB\n", MEMORY_SIZE);
    printf("Used Memory  : %d MB\n", usedMemory);
    printf("Free Memory  : %d MB\n", MEMORY_SIZE - usedMemory);

    printf("\nMemory Allocation Table:\n");

    printf("%-12s %-15s %-15s\n",
           "PROCESS", "SIZE (MB)", "STATUS");

    printf("---------------------------------------------\n");

    for (i = 0; i < memoryBlockCount; i++) {

        printf("%-12d %-15d %-15s\n",
               memoryBlocks[i].processID,
               memoryBlocks[i].size,
               memoryBlocks[i].allocated ?
               "ALLOCATED" : "FREE");
    }
}

void memoryMenu() {
    int choice;

    do {
        printf("\n============================================================\n");
        printf("                    MEMORY MANAGER\n");
        printf("============================================================\n");

        printf("1. Allocate Memory\n");
        printf("2. Deallocate Memory\n");
        printf("3. Display Memory\n");
        printf("4. Back\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                allocateMemory();
                pauseScreen();
                break;

            case 2:
                deallocateMemory();
                pauseScreen();
                break;

            case 3:
                displayMemory();
                pauseScreen();
                break;

            case 4:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 4);
}

/* ---------------- FILE MANAGEMENT ---------------- */

void createFile() {
    if (fileCount >= MAX_FILES) {
        printf("\nFile limit reached.\n");
        return;
    }

    printf("\nEnter file name: ");
    scanf("%49s", files[fileCount].name);

    clearInputBuffer();

    printf("Enter file content: ");
    fgets(files[fileCount].content,
          sizeof(files[fileCount].content),
          stdin);

    files[fileCount].content[
        strcspn(files[fileCount].content, "\n")
    ] = '\0';

    files[fileCount].size =
        strlen(files[fileCount].content);

    files[fileCount].exists = 1;

    fileCount++;

    printf("\nFile created successfully.\n");
}

void listFiles() {
    int i;

    printf("\n============================================================\n");
    printf("                     FILE MANAGER\n");
    printf("============================================================\n");

    printf("%-25s %-15s %-15s\n",
           "FILE NAME", "SIZE", "STATUS");

    printf("------------------------------------------------------------\n");

    for (i = 0; i < fileCount; i++) {

        if (files[i].exists) {

            printf("%-25s %-15d %-15s\n",
                   files[i].name,
                   files[i].size,
                   "AVAILABLE");
        }
    }
}

void readFile() {
    char name[50];
    int i;

    printf("\nEnter file name: ");
    scanf("%49s", name);

    for (i = 0; i < fileCount; i++) {

        if (files[i].exists &&
            strcmp(files[i].name, name) == 0) {

            printf("\nFile Content:\n");
            printf("------------------------------------\n");
            printf("%s\n", files[i].content);

            return;
        }
    }

    printf("\nFile not found.\n");
}

void writeFile() {
    char name[50];
    int i;

    printf("\nEnter file name: ");
    scanf("%49s", name);

    for (i = 0; i < fileCount; i++) {

        if (files[i].exists &&
            strcmp(files[i].name, name) == 0) {

            clearInputBuffer();

            printf("Enter new content: ");

            fgets(files[i].content,
                  sizeof(files[i].content),
                  stdin);

            files[i].content[
                strcspn(files[i].content, "\n")
            ] = '\0';

            files[i].size =
                strlen(files[i].content);

            printf("\nFile updated successfully.\n");

            return;
        }
    }

    printf("\nFile not found.\n");
}

void deleteFile() {
    char name[50];
    int i;

    printf("\nEnter file name: ");
    scanf("%49s", name);

    for (i = 0; i < fileCount; i++) {

        if (files[i].exists &&
            strcmp(files[i].name, name) == 0) {

            files[i].exists = 0;

            printf("\nFile deleted successfully.\n");

            return;
        }
    }

    printf("\nFile not found.\n");
}

void fileMenu() {
    int choice;

    do {

        printf("\n============================================================\n");
        printf("                      FILE MANAGER\n");
        printf("============================================================\n");

        printf("1. Create File\n");
        printf("2. Read File\n");
        printf("3. Write File\n");
        printf("4. Delete File\n");
        printf("5. List Files\n");
        printf("6. Back\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                createFile();
                pauseScreen();
                break;

            case 2:
                readFile();
                pauseScreen();
                break;

            case 3:
                writeFile();
                pauseScreen();
                break;

            case 4:
                deleteFile();
                pauseScreen();
                break;

            case 5:
                listFiles();
                pauseScreen();
                break;

            case 6:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 6);
}

/* ---------------- SYSTEM INFORMATION ---------------- */

void systemInformation() {

    printf("\n============================================================\n");
    printf("                  SYSTEM INFORMATION\n");
    printf("============================================================\n");

    printf("\nOperating System : Windows OS Simulator");
    printf("\nArchitecture     : Simulated 64-bit");
    printf("\nCPU              : Simulated Multi-Core CPU");
    printf("\nTotal Memory     : %d MB", MEMORY_SIZE);
    printf("\nUsed Memory      : %d MB", usedMemory);
    printf("\nFree Memory      : %d MB", MEMORY_SIZE - usedMemory);
    printf("\nTotal Processes  : %d", processCount);
    printf("\nTotal Files      : %d", fileCount);

    printf("\n\nOS Concepts Simulated:");
    printf("\n- Process Management");
    printf("\n- CPU Scheduling");
    printf("\n- Memory Management");
    printf("\n- File Management");
    printf("\n- Process States");
    printf("\n- Multitasking");
}

/* ---------------- WINDOWS START MENU ---------------- */

void processMenu() {
    int choice;

    do {

        printf("\n============================================================\n");
        printf("                   WINDOWS PROCESS MANAGER\n");
        printf("============================================================\n");

        printf("1. Create Process\n");
        printf("2. Display Processes\n");
        printf("3. Change Process State\n");
        printf("4. Terminate Process\n");
        printf("5. Back\n");

        printf("\nEnter choice: ");

        if (scanf("%d",&choice)!=1){
            printf("invalid input!please enter number from 1 to 5");
    
            continue;
        }

        switch (choice) {

            case 1:
                createProcess();
                pauseScreen();
                break;

            case 2:
                displayProcesses();
                pauseScreen();
                break;

            case 3:
                changeProcessState();
                pauseScreen();
                break;

            case 4:
                terminateProcess();
                pauseScreen();
                break;

            case 5:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 5);
}

/* ---------------- MAIN FUNCTION ---------------- */

int main() {

    int choice;

    printf("\n\n");
    printf("============================================================\n");
    printf("                                                            \n");
    printf("              WINDOWS OPERATING SYSTEM                     \n");
    printf("                    WORKING MODEL                          \n");
    printf("                                                            \n");
    printf("============================================================\n");

    printf("\nThis program is a simulation of Windows OS concepts.");
    printf("\nIt demonstrates process, CPU, memory and file management.\n");

    pauseScreen();

    do {

        printf("\n\n");
        printf("============================================================\n");
        printf("                     WINDOWS DESKTOP\n");
        printf("============================================================\n");

        printf("\n                  [ WINDOWS OS ]\n\n");

        printf("1. Process Manager\n");
        printf("2. CPU Scheduling\n");
        printf("3. Memory Manager\n");
        printf("4. File Manager\n");
        printf("5. System Information\n");
        printf("6. Shutdown\n");

        printf("\n------------------------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                processMenu();
                break;

            case 2:
                schedulingMenu();
                break;

            case 3:
                memoryMenu();
                break;

            case 4:
                fileMenu();
                break;

            case 5:
                systemInformation();
                pauseScreen();
                break;

            case 6:
                printf("\n");
                printf("============================================================\n");
                printf("              Windows OS Simulator Shutting Down...\n");
                printf("============================================================\n");

                printf("\nSaving simulated system state...");
                printf("\nClosing processes...");
                printf("\nReleasing memory...");
                printf("\nClosing file manager...");

                printf("\n\nWindows OS Simulator has been shut down.\n");
                break;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }

    } while (choice != 6);

    return 0;
}
