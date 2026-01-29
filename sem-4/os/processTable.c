#include <stdio.h>
#include <string.h>

typedef enum {
    NEW,
    READY,
    RUNNING,
    TERMINATED
} ProcessState;

typedef struct {
    int pid;
    int at;   // Arrival Time
    int bt;   // Burst Time
    int ct;   // Completion Time
    int tat;  // Turnaround Time
    int wt;   // Waiting Time
    ProcessState state;
} Process;

/* ---------------- Utility Functions ---------------- */

const char* state_to_str(ProcessState s) {
    switch (s) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

void print_process_table(Process p[], int n) {
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tSTATE\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%s\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt,
               state_to_str(p[i].state));
    }
}

/* ---------------- Sorting Helpers ---------------- */

void sort_by_arrival(Process p[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (p[j].at > p[j+1].at) {
                Process temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }
}

void sort_by_burst(Process p[], int n, int start) {
    for (int i = start; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            if (p[i].bt > p[j].bt) {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
}

/* ---------------- FCFS Scheduling ---------------- */

void fcfs_scheduling(Process p[], int n) {
    int time = 0;
    sort_by_arrival(p, n);

    for (int i = 0; i < n; i++) {
        if (time < p[i].at)
            time = p[i].at;

        p[i].state = RUNNING;
        time += p[i].bt;
        p[i].ct = time;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
        p[i].state = TERMINATED;
    }

    printf("\n===== FCFS Scheduling =====");
    print_process_table(p, n);
}

/* ---------------- Non-Preemptive SJF ---------------- */

void sjf_non_preemptive(Process p[], int n) {
    int time = 0;
    sort_by_arrival(p, n);

    for (int i = 0; i < n; i++) {
        int idx = -1;
        int min_bt = 1e9;

        for (int j = i; j < n; j++) {
            if (p[j].at <= time && p[j].bt < min_bt) {
                min_bt = p[j].bt;
                idx = j;
            }
        }

        if (idx == -1) {
            time++;
            i--;
            continue;
        }

        Process temp = p[i];
        p[i] = p[idx];
        p[idx] = temp;

        p[i].state = RUNNING;
        time += p[i].bt;
        p[i].ct = time;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
        p[i].state = TERMINATED;
    }

    printf("\n===== Non-Preemptive SJF =====");
    print_process_table(p, n);
}

/* ---------------- Main ---------------- */

int main() {
    Process p[] = {
        {1, 0, 3, 0, 0, 0, NEW},
        {2, 1, 4, 0, 0, 0, NEW},
        {3, 2, 2, 0, 0, 0, NEW},
        {4, 3, 3, 0, 0, 0, NEW}
    };

    int n = sizeof(p) / sizeof(p[0]);

    print_process_table(p,n);

    Process p1[n], p2[n];
    memcpy(p1, p, sizeof(p));
    memcpy(p2, p, sizeof(p));

    fcfs_scheduling(p1, n);
    sjf_non_preemptive(p2, n);

    return 0;
}

