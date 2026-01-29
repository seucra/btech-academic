#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
using namespace std;

// ---------------- Process State ----------------
enum ProcessState { NEW, READY, WAITING, RUNNING, TERMINATED };

string stateToStr(ProcessState s) {
    switch (s) {
        case NEW: return "NEW";
        case READY: return "READY";
        case WAITING: return "WAITING";
        case RUNNING: return "RUNNING";
        case TERMINATED: return "TERMINATED";
        default: return "UNK";
    }
}

// ---------------- Process & Gantt ----------------
struct Gantt {
    int pid, start, end;
};

struct Process {
    int pid;
    int at;           // Arrival Time
    int bt;           // Burst Time
    int rt;           // Remaining Time
    int ct;           // Completion Time
    int tat;          // Turnaround Time
    int wt;           // Waiting Time
    int priority;     // Priority (lower number = higher)
    ProcessState state;
};

// ---------------- Utility Functions ----------------
void printGantt(const vector<Gantt>& g) {
    cout << "\nGantt Chart:\n|";
    for (auto &x : g) cout << " P" << x.pid << " |";
    cout << "\n";
    for (auto &x : g) cout << x.start << setw(5);
    cout << g.back().end << "\n";
}

void printTable(const vector<Process>& p) {
    double awt = 0, atat = 0;
    cout << "\nPID AT BT CT TAT WT STATE\n";
    for (auto &x : p) {
        cout << x.pid << "  " << x.at << "  " << x.bt << "  "
             << x.ct << "  " << x.tat << "  "
             << x.wt << "  " << stateToStr(x.state) << "\n";
        awt += x.wt;
        atat += x.tat;
    }
    cout << "Average WT  = " << awt / p.size() << "\n";
    cout << "Average TAT = " << atat / p.size() << "\n";
}

// ---------------- FCFS ----------------
void fcfs(vector<Process> p) {
    vector<Gantt> g;
    int time = 0;

    sort(p.begin(), p.end(), [](auto &a, auto &b){ return a.at < b.at; });

    for (auto &x : p) {
        if (time < x.at) time = x.at;
        x.state = RUNNING;
        g.push_back({x.pid, time, time + x.bt});
        time += x.bt;
        x.ct = time;
        x.tat = x.ct - x.at;
        x.wt = x.tat - x.bt;
        x.state = TERMINATED;
    }

    cout << "\n===== FCFS =====";
    printGantt(g);
    printTable(p);
}

// ---------------- SJF Non-Preemptive ----------------
void sjf(vector<Process> p) {
    vector<Gantt> g;
    int time = 0, done = 0, n = p.size();

    while (done < n) {
        int idx = -1, mn = 1e9;
        for (int i = 0; i < n; i++) {
            if (p[i].state != TERMINATED && p[i].at <= time && p[i].bt < mn) {
                mn = p[i].bt;
                idx = i;
            }
            if (p[i].state == NEW && p[i].at <= time)
                p[i].state = WAITING;
        }

        if (idx == -1) { time++; continue; }

        p[idx].state = RUNNING;
        g.push_back({p[idx].pid, time, time + p[idx].bt});
        time += p[idx].bt;

        p[idx].ct = time;
        p[idx].tat = time - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].state = TERMINATED;
        done++;
    }

    cout << "\n===== SJF (Non-Preemptive) =====";
    printGantt(g);
    printTable(p);
}

// ---------------- SRTN (Preemptive SJF) ----------------
void srtn(vector<Process> p) {
    vector<Gantt> g;
    int time = 0, done = 0, n = p.size();

    for (auto &x : p) x.rt = x.bt;

    int last = -1, start = 0;

    while (done < n) {
        int idx = -1, mn = 1e9;
        for (int i = 0; i < n; i++) {
            if (p[i].rt > 0 && p[i].at <= time) {
                if (p[i].rt < mn) {
                    mn = p[i].rt;
                    idx = i;
                }
                if (p[i].state != RUNNING && p[i].state != TERMINATED)
                    p[i].state = WAITING;
            }
        }

        if (idx == -1) { time++; continue; }

        if (last != idx) {
            if (last != -1 && p[last].state != TERMINATED)
                g.push_back({p[last].pid, start, time});
            start = time;
            last = idx;
        }

        p[idx].state = RUNNING;
        p[idx].rt--;
        time++;

        if (p[idx].rt == 0) {
            g.push_back({p[idx].pid, start, time});
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            p[idx].state = TERMINATED;
            done++;
            last = -1;
        }
    }

    cout << "\n===== SRTN (Preemptive SJF) =====";
    printGantt(g);
    printTable(p);
}

// ---------------- Round Robin ----------------
void roundRobin(vector<Process> p, int tq) {
    vector<Gantt> g;
    queue<int> q;
    int time = 0, done = 0, n = p.size();

    for (auto &x : p) x.rt = x.bt;

    // initialize ready queue
    for (int i = 0; i < n; i++)
        if (p[i].at <= time) { p[i].state = READY; q.push(i); }

    while (done < n) {
        if (q.empty()) { time++; 
            for (int i = 0; i < n; i++)
                if (p[i].state == NEW && p[i].at <= time) { p[i].state = READY; q.push(i); }
            continue; 
        }

        int i = q.front(); q.pop();
        p[i].state = RUNNING;

        int exec = min(tq, p[i].rt);
        g.push_back({p[i].pid, time, time + exec});
        time += exec;
        p[i].rt -= exec;

        for (int j = 0; j < n; j++)
            if (p[j].state == NEW && p[j].at <= time) {
                p[j].state = READY;
                q.push(j);
            }

        if (p[i].rt > 0) {
            p[i].state = READY;
            q.push(i);
        } else {
            p[i].ct = time;
            p[i].tat = p[i].ct - p[i].at;
            p[i].wt = p[i].tat - p[i].bt;
            p[i].state = TERMINATED;
            done++;
        }
    }

    cout << "\n===== Round Robin (TQ=" << tq << ") =====";
    printGantt(g);
    printTable(p);
}

// ---------------- Priority Scheduling (Non-Preemptive) ----------------
void priorityScheduling(vector<Process> p) {
    vector<Gantt> g;
    int time = 0, done = 0, n = p.size();

    while (done < n) {
        int idx = -1, pr = 1e9;
        for (int i = 0; i < n; i++) {
            if (p[i].state != TERMINATED && p[i].at <= time && p[i].priority < pr) {
                pr = p[i].priority;
                idx = i;
            }
            if (p[i].state == NEW && p[i].at <= time)
                p[i].state = WAITING;
        }

        if (idx == -1) { time++; continue; }

        p[idx].state = RUNNING;
        g.push_back({p[idx].pid, time, time + p[idx].bt});
        time += p[idx].bt;

        p[idx].ct = time;
        p[idx].tat = time - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].state = TERMINATED;
        done++;
    }

    cout << "\n===== Priority Scheduling =====";
    printGantt(g);
    printTable(p);
}

// ---------------- Main ----------------
int main() {
    vector<Process> p = {
        {1, 0, 5, 0, 0, 0, 0, 2, NEW},
        {2, 1, 3, 0, 0, 0, 0, 1, NEW},
        {3, 2, 4, 0, 0, 0, 0, 3, NEW},
        {4, 3, 2, 0, 0, 0, 0, 2, NEW}
    };

    fcfs(p);
    sjf(p);
    srtn(p);
    roundRobin(p, 2);
    priorityScheduling(p);

    return 0;
}

