# Lock-Free Data Structures Library

## Overview
This repository contains a collection of **lock-free data structures** implemented in C++. Lock-free data structures provide improved parallel processing efficiency by eliminating the need for mutual exclusion mechanisms like locks, thereby reducing contention and enhancing performance.

## Features
- **Lock-Free Queue**
- **Lock-Free Priority Queue**
- **Lock-Free Ring Buffer**
- **Lock-Free Linked List**

These data structures are implemented using **C++ atomic operations** to ensure thread safety and high performance in concurrent environments.

## Getting Started

### Prerequisites
- C++17 or later
- CMake 3.10 or later

### Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/mohit-jethwani49/Lock-Free-DS_lib.git
   cd Lock-Free-DS_lib
   ```
2. Build the project using CMake:
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```

### Usage
Each data structure has a corresponding test file that demonstrates its usage. Example:
```cpp
LockFreeQueue<int> queue;
queue.enqueue(10);
int value;
bool success = queue.dequeue(value);
```
Run the test executables:
```sh
./build/test_queue
./build/test_priority_queue
./build/test_ring_buffer
./build/test_linked_list
```

## Performance Analysis
The library is designed to optimize concurrent access patterns, reducing contention and improving scalability. Benchmark tests can be run to measure performance improvements over traditional lock-based implementations.

## Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create a new branch (`feature-branch`)
3. Commit your changes
4. Push to the branch and create a Pull Request

