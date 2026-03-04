
#include <bits/stdc++.h>
#include "parser.h"
using namespace std;

// Possible algorithm names
const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";

// Mapping algorithm index → name
const string ALGORITHMS[11] = {
    "", "FCFS", "RR-", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "AGING", "CFS", "EDF"
};

// ----------------- Basic field extractors ------------------
string getProcessName(const tuple<string, int, int, int> &a) { return get<0>(a); }
int getArrivalTime(const tuple<string, int, int, int> &a) { return get<1>(a); }
int getServiceTime(const tuple<string, int, int, int> &a) { return get<2>(a); }
int getDeadline(const tuple<string, int, int, int> &a) { return get<3>(a); }
int getPriorityLevel(const tuple<string, int, int, int> &a) { return get<2>(a); }

// HRRN formula
inline double calculate_response_ratio(int wait_time, int service_time) {
    return (wait_time + service_time) * 1.0 / service_time;
}

// ------------------------------------------------------------
// Clears timeline before running each scheduling algorithm
// ------------------------------------------------------------
void clear_timeline() {
    for (int i = 0; i < last_instant; i++)
        for (int j = 0; j < process_count; j++)
            timeline[i][j] = ' ';
}

// ------------------------------------------------------------
// After scheduling, fill remaining idle slots with '.' (waiting)
// ------------------------------------------------------------
void fillInWaitTime() {
    for (int i = 0; i < process_count; i++) {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++) {
            if (timeline[k][i] != '*')      // not running
                timeline[k][i] = '.';       // waiting
        }
    }
}

// ------------------------------------------------------------
// FCFS Scheduling (non-preemptive)
// ------------------------------------------------------------
void firstComeFirstServe() {
    int time = getArrivalTime(processes[0]);

    for (int i = 0; i < process_count; i++) {
        int idx = i;
        int arrival = getArrivalTime(processes[i]);
        int service = getServiceTime(processes[i]);

        finishTime[idx] = time + service;
        turnAroundTime[idx] = finishTime[idx] - arrival;
        normTurn[idx] = turnAroundTime[idx] * 1.0 / service;

        // Running (* marks execution)
        for (int t = time; t < finishTime[idx] && t < last_instant; t++)
            timeline[t][idx] = '*';

        // Waiting (.) marks wait before start
        for (int t = arrival; t < time && t < last_instant; t++)
            timeline[t][idx] = '.';

        time += service;   // move to next
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// Round Robin Scheduling (preemptive, time-slicing)
// ------------------------------------------------------------
void roundRobin(int originalQuantum) {
    queue<int> q;
    vector<int> remaining(process_count);

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    int time = 0, j = 0;
    vector<bool> inQueue(process_count, false);

    // Add all processes arriving at time 0
    while (j < process_count && getArrivalTime(processes[j]) == 0) {
        q.push(j);
        inQueue[j] = true;
        j++;
    }

    // Main RR loop
    while (time < last_instant && (!q.empty() || j < process_count)) {

        // Add processes that arrived by current time
        while (j < process_count && getArrivalTime(processes[j]) <= time) {
            if (!inQueue[j]) {
                q.push(j);
                inQueue[j] = true;
            }
            j++;
        }

        if (q.empty()) { time++; continue; }

        int idx = q.front();
        q.pop();

        int quantum = min(originalQuantum, remaining[idx]);
        int ran = 0;

        // Execute for min(quantum, remaining)
        while (ran < quantum && remaining[idx] > 0 && time < last_instant) {
            timeline[time][idx] = '*';
            time++;
            ran++;
            remaining[idx]--;
        }

        // Finished
        if (remaining[idx] == 0) {
            finishTime[idx] = time;
            turnAroundTime[idx] = time - getArrivalTime(processes[idx]);
            normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
        } else {
            // Put back for another round
            q.push(idx);
        }
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// SPN - Shortest Process Next (non-preemptive)
// ------------------------------------------------------------
void shortestProcessNext() {
    int time = 0;
    vector<int> remaining(process_count);
    vector<bool> done(process_count, false);

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    while (time < last_instant) {

        // Pick shortest process among arrived ones
        int idx = -1, min_svc = INT_MAX;
        for (int i = 0; i < process_count; i++) {
            if (!done[i] && getArrivalTime(processes[i]) <= time && remaining[i] < min_svc && remaining[i] > 0) {
                idx = i;
                min_svc = remaining[i];
            }
        }

        if (idx == -1) { time++; continue; }

        // Run completely
        for (int t = 0; t < remaining[idx] && time < last_instant; t++, time++)
            timeline[time][idx] = '*';

        finishTime[idx] = time;
        turnAroundTime[idx] = time - getArrivalTime(processes[idx]);
        normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
        done[idx] = true;
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// SRT - Shortest Remaining Time (preemptive SPN)
// ------------------------------------------------------------
void shortestRemainingTime() {
    int time = 0;
    vector<int> remaining(process_count);
    vector<bool> done(process_count, false);

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    while (time < last_instant) {

        // Find job with smallest remaining time
        int idx = -1, best = INT_MAX;
        for (int i = 0; i < process_count; i++) {
            if (!done[i] && getArrivalTime(processes[i]) <= time && remaining[i] > 0 && remaining[i] < best) {
                idx = i;
                best = remaining[i];
            }
        }

        if (idx == -1) { time++; continue; }

        timeline[time][idx] = '*';
        remaining[idx]--;

        // If finished
        if (remaining[idx] == 0) {
            finishTime[idx] = time + 1;
            turnAroundTime[idx] = finishTime[idx] - getArrivalTime(processes[idx]);
            normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
            done[idx] = true;
        }

        time++;
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// HRRN - Highest Response Ratio Next
// ------------------------------------------------------------
void highestResponseRatioNext() {
    vector<int> remaining(process_count);
    vector<bool> done(process_count, false);

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    int time = 0;

    while (time < last_instant) {

        double max_ratio = -1;
        int idx = -1;

        // Select job with max response ratio
        for (int i = 0; i < process_count; i++) {
            if (!done[i] && getArrivalTime(processes[i]) <= time && remaining[i] > 0) {
                int wait = time - getArrivalTime(processes[i]);
                double ratio = calculate_response_ratio(wait, remaining[i]);
                if (ratio > max_ratio) {
                    max_ratio = ratio;
                    idx = i;
                }
            }
        }

        if (idx == -1) { time++; continue; }

        // Run to completion
        while (remaining[idx] > 0 && time < last_instant) {
            timeline[time][idx] = '*';
            remaining[idx]--;
            time++;
        }

        finishTime[idx] = time;
        turnAroundTime[idx] = time - getArrivalTime(processes[idx]);
        normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
        done[idx] = true;
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// FB-1 - Multi-Level Feedback Queue (1 unit quantum per queue)
// ------------------------------------------------------------
void feedbackQ1() {
    int time = 0, j = 0;
    vector<int> remaining(process_count);

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    vector<queue<int>> queues(10);  // 10 levels

    // Add processes with arrival = 0
    while (j < process_count && getArrivalTime(processes[j]) == 0)
        queues[0].push(j++);

    while (time < last_instant) {

        // Add arriving processes
        while (j < process_count && getArrivalTime(processes[j]) <= time)
            queues[0].push(j++);

        // Find first non-empty queue
        int lvl = -1;
        for (int x = 0; x < 10; x++)
            if (!queues[x].empty()) { lvl = x; break; }

        if (lvl == -1) { time++; continue; }

        int idx = queues[lvl].front();
        queues[lvl].pop();

        timeline[time][idx] = '*';
        remaining[idx]--;

        if (remaining[idx] == 0) {
            finishTime[idx] = time + 1;
            turnAroundTime[idx] = finishTime[idx] - getArrivalTime(processes[idx]);
            normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
        } else {
            // Move to next queue level
            queues[min(lvl + 1, 9)].push(idx);
        }

        time++;
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// FB-2i - Multi-Level Feedback Queue with increasing quantum (2^i)
// ------------------------------------------------------------
void feedbackQ2i() {
    int time = 0, j = 0;
    vector<int> remaining(process_count);

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    vector<queue<int>> queues(10);

    // Add processes arriving at time 0
    while (j < process_count && getArrivalTime(processes[j]) == 0)
        queues[0].push(j++);

    while (time < last_instant) {

        // Add processes
        while (j < process_count && getArrivalTime(processes[j]) <= time)
            queues[0].push(j++);

        // Determine queue
        int lvl = -1;
        for (int k = 0; k < 10; k++)
            if (!queues[k].empty()) { lvl = k; break; }

        if (lvl == -1) { time++; continue; }

        int idx = queues[lvl].front();
        queues[lvl].pop();

        int quantum = min(1 << lvl, remaining[idx]);
        int used = 0;

        while (used < quantum && time < last_instant && remaining[idx] > 0) {
            timeline[time][idx] = '*';
            time++;
            remaining[idx]--;
            used++;
        }

        if (remaining[idx] == 0) {
            finishTime[idx] = time;
            turnAroundTime[idx] = time - getArrivalTime(processes[idx]);
            normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
        } else {
            queues[min(lvl + 1, 9)].push(idx);
        }
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// AGING scheduler (priority increases with waiting)
// ------------------------------------------------------------
void aging(int quantum) {
    int time = 0;
    vector<int> remaining(process_count);
    vector<int> prio(process_count);

    for (int i = 0; i < process_count; i++) {
        remaining[i] = getServiceTime(processes[i]);
        prio[i] = getPriorityLevel(processes[i]);
    }

    while (time < last_instant) {

        // Select process with highest priority
        int idx = -1, best = -1;
        for (int i = 0; i < process_count; i++) {
            if (remaining[i] > 0 && getArrivalTime(processes[i]) <= time && prio[i] > best) {
                best = prio[i];
                idx = i;
            }
        }

        if (idx == -1) { time++; continue; }

        int run = min(quantum, remaining[idx]);

        for (int t = 0; t < run && time < last_instant && remaining[idx] > 0; t++, time++) {
            timeline[time][idx] = '*';
            remaining[idx]--;
        }

        // If finished
        if (remaining[idx] == 0) {
            finishTime[idx] = time;
            turnAroundTime[idx] = time - getArrivalTime(processes[idx]);
            normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
            prio[idx] = getPriorityLevel(processes[idx]);
        }

        // Increase priority of waiting processes
        for (int i = 0; i < process_count; i++)
            if (i != idx && getArrivalTime(processes[i]) <= time)
                prio[i]++;
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// CFS - Completely Fair Scheduler (virtual runtime ordering)
// ------------------------------------------------------------
void cfs_scheduler() {
    vector<int> remaining(process_count);
    set<pair<int, int>> ready_set;     // (virtual runtime, process idx)

    int j = 0;

    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    for (int time = 0; time < last_instant; time++) {

        // Add all processes that arrived
        while (j < process_count && getArrivalTime(processes[j]) <= time)
            ready_set.insert({virtualRuntime[j], j}), j++;

        if (!ready_set.empty()) {
            auto it = ready_set.begin();
            int idx = it->second;
            ready_set.erase(it);

            timeline[time][idx] = '*';
            virtualRuntime[idx]++;
            remaining[idx]--;

            if (remaining[idx] == 0) {
                finishTime[idx] = time + 1;
                turnAroundTime[idx] = finishTime[idx] - getArrivalTime(processes[idx]);
                normTurn[idx] = turnAroundTime[idx] * 1.0 / getServiceTime(processes[idx]);
            } else
                ready_set.insert({virtualRuntime[idx], idx});
        }
    }

    virtualRuntime.assign(process_count, 0);
    fillInWaitTime();
}

// ------------------------------------------------------------
// EDF - Earliest Deadline First
// ------------------------------------------------------------
void edf_scheduler() {
    vector<int> remaining(process_count);
    for (int i = 0; i < process_count; i++)
        remaining[i] = getServiceTime(processes[i]);

    for (int time = 0; time < last_instant; time++) {

        int sel = -1, best_deadline = INT_MAX;

        for (int i = 0; i < process_count; i++) {
            if (remaining[i] > 0 && getArrivalTime(processes[i]) <= time) {
                int d = getDeadline(processes[i]);
                if (d != -1 && d < best_deadline) {
                    best_deadline = d;
                    sel = i;
                }
            }
        }

        if (sel == -1) continue;

        timeline[time][sel] = '*';
        remaining[sel]--;

        if (remaining[sel] == 0) {
            finishTime[sel] = time + 1;
            turnAroundTime[sel] = finishTime[sel] - getArrivalTime(processes[sel]);
            normTurn[sel] = turnAroundTime[sel] * 1.0 / getServiceTime(processes[sel]);
        }
    }

    fillInWaitTime();
}

// ------------------------------------------------------------
// Printing helpers (used in stats mode)
// ------------------------------------------------------------
void printAlgorithm(int algorithm_index) {
    int id = algorithms[algorithm_index].first - '0';

    if (id == 2)
        cout << ALGORITHMS[id] << algorithms[algorithm_index].second << endl;
    else if (id == 9)
        cout << ALGORITHMS[9] << endl;
    else if (algorithms[algorithm_index].first == 'A')
        cout << ALGORITHMS[10] << endl;
    else
        cout << ALGORITHMS[id] << endl;
}

void printProcesses() {
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}

void printArrivalTime() {
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ", getArrivalTime(processes[i]));
    cout << "|\n";
}

void printServiceTime() {
    cout << "Service    |";
    for (int i = 0; i < process_count; i++)
        printf("%3d  |", getServiceTime(processes[i]));
    cout << " Mean|\n";
}

void printFinishTime() {
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ", finishTime[i]);
    cout << "|-----|\n";
}

void printTurnAroundTime() {
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++) {
        printf("%3d  |", turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    printf(" %2.2f|\n", (1.0 * sum / process_count));
}

void printNormTurn() {
    cout << "NormTurn   |";
    float sum = 0;
    for (int i = 0; i < process_count; i++) {
        printf(" %2.2f|", normTurn[i]);
        sum += normTurn[i];
    }
    printf(" %2.2f|\n", (sum / process_count));
}

void printStats(int algorithm_index) {
    printAlgorithm(algorithm_index);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

// ------------------------------------------------------------
// Timeline output (used in TRACE mode)
// ------------------------------------------------------------
void printTimeline(int algorithm_index) {
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10 << " ";
    cout << "\n--------------------------------------------\n";

    for (int i = 0; i < process_count; i++) {
        cout << getProcessName(processes[i]) << "     |";
        for (int t = 0; t < last_instant; t++)
            cout << timeline[t][i] << "|";
        cout << " \n";
    }

    cout << "--------------------------------------------\n";
}

// ------------------------------------------------------------
// Dispatcher to execute chosen algorithm
// ------------------------------------------------------------
void execute_algorithm(char algorithm_id, int quantum, string operation) {
    switch (algorithm_id) {
        case '1': firstComeFirstServe(); break;
        case '2': roundRobin(quantum); break;
        case '3': shortestProcessNext(); break;
        case '4': shortestRemainingTime(); break;
        case '5': highestResponseRatioNext(); break;
        case '6': feedbackQ1(); break;
        case '7': feedbackQ2i(); break;
        case '8': aging(quantum); break;
        case '9': cfs_scheduler(); break;
        case 'A': edf_scheduler(); break;
    }
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {
    parse();  // read input

    for (int idx = 0; idx < (int)algorithms.size(); idx++) {
        clear_timeline();

        execute_algorithm(algorithms[idx].first, algorithms[idx].second, operation);

        if (operation == TRACE)
            printTimeline(idx);
        else if (operation == SHOW_STATISTICS)
            printStats(idx);

        cout << "\n";
    }

    return 0;
}
