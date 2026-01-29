#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

enum ProcessState {
    NEW,
    READY,
    RUNNING,
    TERMINATED
};

struct Process {
    int pid;
    int at;     // Arrival Time
    int bt;     // Burst Time
    int rt;     // Remaining Time
    int ct;     // Completion Time
    int tat;    // Turnaround Time
    int wt;     // Waiting Time
    ProcessState state;
};

/* ---------------- Utility ---------------- */

string stateToStr(ProcessState s) {
    switch (s) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

void printTable(const vector<Process>& p) {
    cout << "\nPID  AT  BT  CT  TAT  WT  STATE\n";
    for (const auto& pr : p) {
        cout << setw(3) << pr.pid << "  "
             << setw(2) << pr.at  << "  "
             << setw(2) << pr.bt  << "  "
             << setw(2) << pr.ct  << "  "
             << setw(3) << pr.tat << "  "
             << setw(2) << pr.wt  << "  "
             << pr.state << "\n";
    }
}

/* ---------------- FCFS ---------------- */

void fcfs(vector<Process> p) {
    int time = 0;

    sort(p.begin(), p.end(), [](auto &a, auto &b) {
        return a.at < b.at;
    });

    for (auto &pr : p) {
        if (time < pr.at)
            time = pr.at;

        pr.state = RUNNING;
        time += pr.bt;

        pr.ct = time;
        pr.tat = pr.ct - pr.at;
        pr.wt = pr.tat - pr.bt;
        pr.state = TERMINATED;
    }

    cout << "\n===== FCFS =====";
    printTable(p);
}

/* ---------------- SJF Non-Preemptive ---------------- */

void sjfNonPreemptive(vector<Process> p) {
    int time = 0, completed = 0;
    int n = p.size();

    while (completed < n) {
        int idx = -1;
        int minBT = 1e9;

        for (int i = 0; i < n; i++) {
            if (p[i].state != TERMINATED &&
                p[i].at <= time &&
                p[i].bt < minBT) {
                minBT = p[i].bt;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        p[idx].state = RUNNING;
        time += p[idx].bt;

        p[idx].ct = time;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].state = TERMINATED;

        completed++;
    }

    cout << "\n===== SJF (Non-Preemptive) =====";
    printTable(p);
}

/* ---------------- SRTN (SJF Preemptive) ---------------- */

void srtn(vector<Process> p) {
    int time = 0, completed = 0;
    int n = p.size();

    for (auto &pr : p)
        pr.rt = pr.bt;

    while (completed < n) {
        int idx = -1;
        int minRT = 1e9;

        for (int i = 0; i < n; i++) {
            if (p[i].at <= time &&
                p[i].rt > 0 &&
                p[i].rt < minRT) {
                minRT = p[i].rt;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        p[idx].state = RUNNING;
        p[idx].rt--;
        time++;

        if (p[idx].rt == 0) {
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            p[idx].state = TERMINATED;
            completed++;
        }
    }

    cout << "\n===== SRTN (SJF Preemptive) =====";
    printTable(p);
}

/* ---------------- Main ---------------- */

int main() {
    vector<Process> processes = {
        {1, 0, 3, 0, 0, 0, 0, NEW},
        {2, 1, 4, 0, 0, 0, 0, NEW},
        {3, 2, 2, 0, 0, 0, 0, NEW},
        {4, 3, 3, 0, 0, 0, 0, NEW}
    };

    fcfs(processes);
    sjfNonPreemptive(processes);
    srtn(processes);

    return 0;
}

