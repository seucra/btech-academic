#include <stdio.h>
#include <string.h>

typedef struct enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    int pid;
    ProcessState state;
    int at, bt, ct;
}

const char* state_to_str (ProcessState s) {
    Switch (s) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case WAITING: return "WAITING";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

int main() {
    // process - array of structure
    process p[] = {
        {1, NEW, 0, 3, 0},
        {2, NEW, 1, 4, 0},
        {3, NEW, 2, 2, 0},
        {4, NEW, 3, 3, 0}
    };
    
    int n = sizeof(p) / sizeof(p[0]);

    printf("PID\tSTATE\t\tAT\tBT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%-10s\t%d\t%d\n",
               p[i].pid,
               state_to_str(p[i].state),
               p[i].at,
               p[i].bt,
               p[i].ct);
    }

    int st = 0; // System Time

    printf("\t\t===FCFS===");
    
    for (int i=0; i<n; i++){
        for (int j=0; j<n-i-1; j++) {
            if (p[j].bt < p[j+1].bt) {
                process temp = p[j+1];
                p[j+1] = p[j];
                p[j] = temp;
            }
        }
    }

    while (1) {
        for (int i=0; i<n; i++) {
            if (p[i].at < st) {
                p[i].state = READY;
            }
        }
    }

    }

    for (int i = 0; i < n; i++) {
        p
        printf("");
    }

    return 0;
}
