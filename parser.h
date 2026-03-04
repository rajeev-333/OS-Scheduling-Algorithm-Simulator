
#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <bits/stdc++.h>
using namespace std;

// Operation mode: "trace" or "stats"
string operation;

// Total time units to simulate and number of processes
int last_instant, process_count;

// List of algorithms to run: (algorithm_id, quantum)
vector<pair<char, int>> algorithms;

// Stores all processes as tuples: (name, arrival, service, deadline)
vector<tuple<string, int, int, int>> processes;

// Maps process name to index for quick lookup
unordered_map<string,int> processToIndex;

// Virtual runtime for CFS scheduling
vector<int> virtualRuntime;

// Timeline matrix: rows = time, columns = processes
vector<vector<char>> timeline;

// Output metrics per process
vector<int> finishTime;
vector<int> turnAroundTime;
vector<float> normTurn;

// ------------------------------------------------------------
// parse_algorithms: Parses the algorithm list
// Format example: "2-4,5-3,1" meaning RR with quantum 4, HRRN, FCFS
// ------------------------------------------------------------
void parse_algorithms(string algorithm_chunk) {
    stringstream stream(algorithm_chunk);
    while (stream.good()) {
        string temp_str;
        getline(stream, temp_str, ',');       // split by comma
        if (temp_str.empty()) continue;

        stringstream ss(temp_str);
        getline(ss, temp_str, '-');          // algorithm ID
        char algorithm_id = temp_str[0];

        getline(ss, temp_str, '-');          // quantum if exists
        int quantum = temp_str.size() >= 1 ? stoi(temp_str) : -1;

        // store parsed (id, quantum)
        algorithms.push_back( make_pair(algorithm_id, quantum) );
    }
}

// ------------------------------------------------------------
// parse_processes: Reads full process lines
// Expected format: name,arrival,service,deadline(optional)
// ------------------------------------------------------------
void parse_processes() {
    string process_line;

    for (int i = 0; i < process_count; i++) {
        getline(cin >> ws, process_line);

        // Trim whitespace
        size_t start = process_line.find_first_not_of(" \t\r\n");
        size_t end = process_line.find_last_not_of(" \t\r\n");

        if (start == string::npos) {
            cerr << "Error: Empty process line at index " << i << "\n";
            exit(1);
        }
        process_line = process_line.substr(start, end - start + 1);

        stringstream stream(process_line);
        string temp_str;

        // Parse name
        if (!getline(stream, temp_str, ',')) {
            cerr << "Error: Missing process name\n";
            exit(1);
        }
        string process_name = temp_str;

        // Parse arrival
        if (!getline(stream, temp_str, ',')) {
            cerr << "Error: Missing arrival time\n";
            exit(1);
        }
        int arrival_time = stoi(temp_str);

        // Parse service
        if (!getline(stream, temp_str, ',')) {
            cerr << "Error: Missing service time\n";
            exit(1);
        }
        int service_time = stoi(temp_str);

        // Deadline (optional)
        int deadline = -1;
        if (getline(stream, temp_str, ','))
            deadline = stoi(temp_str);

        // Save
        processes.push_back(make_tuple(process_name, arrival_time, service_time, deadline));
        processToIndex[process_name] = i;
    }
}

// ------------------------------------------------------------
// parse(): Reads first header line + process definitions
// ------------------------------------------------------------
void parse() {
    string first_line;
    getline(cin, first_line);

    stringstream header_stream(first_line);
    header_stream >> operation;       // trace or stats

    string algorithm_chunk;
    header_stream >> algorithm_chunk; // algorithm sequence

    header_stream >> last_instant >> process_count;

    parse_algorithms(algorithm_chunk);
    parse_processes();

    finishTime.resize(process_count);
    turnAroundTime.resize(process_count);
    normTurn.resize(process_count);

    // Initialize timeline: time x processes
    timeline.resize(last_instant);
    for (int i = 0; i < last_instant; i++)
        for (int j = 0; j < process_count; j++)
            timeline[i].push_back(' ');

    virtualRuntime.assign(process_count, 0);
}

#endif // PARSER_H_INCLUDED

