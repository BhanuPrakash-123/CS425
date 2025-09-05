# Assignment 4: CS425  
### Code by Aman(220413), Bhanu(220583), Rithwin(220537)  

## Distance Vector & Link State Routing Simulation  

This project simulates two fundamental routing algorithms:  
- **Distance Vector Routing (DVR)** using Bellman-Ford  
- **Link State Routing (LSR)** using Dijkstra's algorithm  
Given an adjacency matrix as input, the program computes optimal paths and displays routing tables for all nodes.  

---  

## Prerequisites  

1. **Linux/WSL Environment**: Required for running C++ programs with Makefile.  
2. **Compiler**: Install g++ using: sudo apt update && sudo apt install g++
3. **Input File**: Ensure your adjacency matrix file follows the specified format (see [Input Format](#input-format)).  

---  

## Method to Run the Project  

1. **Clone the Repository**:  
    git clone https://github.com/privacy-iitk/cs425-2025.git  
    cd cs425-2025

2. **Compile the Code**:  
    make

3. **Run the Simulation**:  
    ./routing_sim inputfile.txt

Replace `inputfile.txt` with your input file.  

---  

## Input Format  
- First line: `n` (number of nodes)  
- Next `n` lines: `n` space-separated integers representing the adjacency matrix.  
- `0`: No link/self-loop  
- `9999`: Unreachable node  

**Example**:  
4  
0 10 100 30  
10 0 20 40  
100 20 0 10  
30 40 10 0  
---  

## Contributions  

- **Aman (220413)**: 33% - Implemented DVR algorithm and README  
- **Bhanu (220583)**: 33% - Debugged LSR and testing  
- **Rithwin (220537)**: 33% - Code formatting and input handling  

---  

### Challenges Faced  

1. **Infinite Loop in DVR**: Initial Bellman-Ford implementation caused infinite loops until convergence checks were added.  
2. **Next-Hop Calculation**: Struggled to track the first hop in LSR paths until using predecessor backtracking.  
3. **Input Parsing**: Handling `9999` as infinity required special checks in Dijkstra's algorithm.  

---  

### Sources Referenced  

- Bellman-Ford Algorithm: GeeksforGeeks  
- Dijkstra's Algorithm: CS425 Lecture Notes 

---  

## Declaration  
We confirm that this implementation is our original work and adheres to academic integrity guidelines. No plagiarism was involved.  

