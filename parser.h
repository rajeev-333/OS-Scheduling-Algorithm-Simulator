#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <bits/stdc++.h>
using namespace std;

string operation;
int last_instant, process_count;
vector<pair<char, int>> algorithms;

// Process tuple: (name, arrival, service, deadline)
vector<tuple<string, int, int, int>> processes;
unordered_map<string,int> processToIndex;
vector<int> virtualRuntime;

vector<vector<char>> timeline;

vector<int> finishTime;
vector<int> turnAroundTime;
vector<float> normTurn;

// Parse algorithm list ([algoID]-quantum,...)
void parse_algorithms(string algorithm_chunk) {
    stringstream stream(algorithm_chunk);
    while (stream.good()) {
        string temp_str;
        getline(stream, temp_str, ',');
        if (temp_str.empty()) continue;
        stringstream ss(temp_str);
        getline(ss, temp_str, '-');
        char algorithm_id = temp_str[0];
        getline(ss, temp_str, '-');
        int quantum = temp_str.size() >= 1 ? stoi(temp_str) : -1;
        algorithms.push_back( make_pair(algorithm_id, quantum) );
    }
}

// Improved parse_processes function reading entire line per process robustly
void parse_processes() {
    string process_line;
    for (int i = 0; i < process_count; i++) {
        getline(cin >> ws, process_line);
        // Trim leading/trailing whitespace
        size_t start = process_line.find_first_not_of(" \t\r\n");
        size_t end = process_line.find_last_not_of(" \t\r\n");
        if (start == string::npos) {
            cerr << "Error: Empty process line at index " << i << "\n";
            exit(1);
        }
        process_line = process_line.substr(start, end - start + 1);

        stringstream stream(process_line);
        string temp_str;

        if (!getline(stream, temp_str, ',')) {
            cerr << "Error: Missing process name in line: " << process_line << "\n";
            exit(1);
        }
        string process_name = temp_str;

        if (!getline(stream, temp_str, ',')) {
            cerr << "Error: Missing arrival time in line: " << process_line << "\n";
            exit(1);
        }
        int arrival_time = stoi(temp_str);

        if (!getline(stream, temp_str, ',')) {
            cerr << "Error: Missing service time in line: " << process_line << "\n";
            exit(1);
        }
        int service_time = stoi(temp_str);

        int deadline = -1;
        if (getline(stream, temp_str, ',')) {
            deadline = stoi(temp_str);
        }

        processes.push_back(make_tuple(process_name, arrival_time, service_time, deadline));
        processToIndex[process_name] = i;
    }
}

// Updated parse reads entire first line then parses tokens safely
void parse() {
    string first_line;
    getline(cin, first_line);
    stringstream header_stream(first_line);

    header_stream >> operation;

    string algorithm_chunk;
    header_stream >> algorithm_chunk;

    header_stream >> last_instant >> process_count;

    parse_algorithms(algorithm_chunk);
    parse_processes();

    finishTime.resize(process_count);
    turnAroundTime.resize(process_count);
    normTurn.resize(process_count);
    timeline.resize(last_instant);
    for (int i = 0; i < last_instant; i++)
        for (int j = 0; j < process_count; j++)
            timeline[i].push_back(' ');

    virtualRuntime.assign(process_count, 0);
}

#endif // PARSER_H_INCLUDED
