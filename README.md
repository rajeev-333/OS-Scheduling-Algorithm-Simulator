# CPU Scheduling Algorithm Simulator

A comprehensive Operating Systems project that simulates classical and modern CPU scheduling algorithms, generates execution timelines, and computes performance metrics such as Turnaround Time and Normalized Turnaround Time.

This simulator supports 10 scheduling algorithms, including advanced schedulers like Completely Fair Scheduler (CFS) and Earliest Deadline First (EDF).

# Features

* Supports 10 CPU Scheduling Algorithms

* Generates Execution Timeline (TRACE mode)

* Computes Performance Metrics (STATS mode)

* Calculates:

   * Finish Time

   * Turnaround Time

   * Normalized Turnaround Time

* Handles arrival time, service time, and deadlines

* Simulates:

   * Preemptive scheduling

   * Non-preemptive scheduling

   * Multi-level feedback queues

   * Real-time deadline scheduling

   * Linux-style fairness scheduling

##  Supported Scheduling Algorithms

| ID | Algorithm |
| :--- | :--- |
| 1 | First Come First Serve (FCFS) |
| 2 | Round Robin (RR) |
| 3 | Shortest Process Next (SPN) |
| 4 | Shortest Remaining Time (SRT) |
| 5 | Highest Response Ratio Next (HRRN) |
| 6 | Feedback Queue (FB-1) |
| 7 | Feedback Queue (FB-2i) |
| 8 | Aging |
| 9 | Completely Fair Scheduler (CFS) |
| A | Earliest Deadline First (EDF) |
## Input Format
     <MODE> <ALGORITHMS> <LAST_INSTANT> <PROCESS_COUNT>
     <PROCESS_NAME>,<ARRIVAL_TIME>,<SERVICE_TIME>[,<DEADLINE>]
     ...
### Example
     TRACE 1,2-2,9,A 20 3
     P1,0,5
     P2,1,3
     P3,2,4,15
## Explanation

 * TRACE → Print execution timeline

 * STATS → Print performance metrics

 * 1,2-2,9,A → Run FCFS, RR (quantum=2), CFS, EDF

 * 20 → Simulation runs until time 20

 * 3 → Number of processes

## How to Compile
     g++ -std=c++17 main.cpp -o scheduler
##  How to Run
     ./scheduler < sample_input.txt
## Output Modes
🔹 TRACE Mode

Displays process execution timeline across time units.

🔹 STATS Mode

Displays:

  * Finish Time

  * Turnaround Time

  * Normalized Turnaround Time

## 🧠 Concepts Demonstrated

    CPU Scheduling Algorithms

    Preemptive vs Non-preemptive Scheduling

    Fairness in Scheduling

    Real-Time Scheduling

    Multi-Level Feedback Queues

    Virtual Runtime Simulation (CFS)

    Starvation Prevention (Aging)

    Performance Metrics Calculation

  ##  Project Structure

```text
OS-Scheduling-Algorithm-Simulator/
├── main.cpp
├── parser.h
├── sample_input.txt
├── sample_output.txt
└── README.md

## Design Highlights

   * Modular input parsing

   * Timeline-based simulation engine

   * Clean separation between scheduling logic and metrics computation

   * Efficient data structures (sets, queues, priority-based selection)

   * Extensible architecture for adding new scheduling algorithms

## Why This Project Stands Out

Unlike basic academic simulators, this project includes:

✅ Linux-inspired Completely Fair Scheduler (CFS)

✅ Real-time Earliest Deadline First (EDF)

✅ Dynamic priority scheduling (Aging)

✅ Multiple Feedback Queue implementations

It demonstrates strong understanding of:

* Operating System internals

* Scheduling theory

* Performance analysis

* System-level simulation design

## Tech Stack

    C++

    STL (vector, set, queue, unordered_map, tuple, etc.)

    Standard Template Library Data Structures

    C++17
