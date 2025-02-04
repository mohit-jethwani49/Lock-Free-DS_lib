#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include "../include/LockBasedLinkedList.hpp"
#include "../include/LockFreeLinkedList.hpp"


void testLockBasedList(LockBasedLinkedList& list, int numThreads) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform insertions for a long time (30 seconds)
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([&list, i]() {
            for (int j = 0; j < 1000; ++j) {  // Each thread performs 1000 inserts
                list.insert(i * 1000 + j, nullptr);
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    
    // Perform searches for a long time (30 seconds)
    threads.clear();
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([&list, i]() {
            for (int j = 0; j < 1000; ++j) {  // Each thread performs 1000 searches
                list.search(i * 1000 + j);
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    
    // Perform deletions for a long time (30 seconds)
    threads.clear();
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([&list, i]() {
            for (int j = 0; j < 1000; ++j) {  // Each thread performs 1000 deletions
                list.deleteNode(i * 1000 + j);
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Lock-Based List operations took: " << duration.count() << " seconds.\n";
}

void testLockFreeList(LinkedList& list, int numThreads) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform insertions for a long time (30 seconds)
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([&list, i]() {
            for (int j = 0; j < 1000; ++j) {  // Each thread performs 1000 inserts
                list.insert(i * 1000 + j, nullptr);
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    
    // Perform searches for a long time (30 seconds)
    threads.clear();
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([&list, i]() {
            for (int j = 0; j < 1000; ++j) {  // Each thread performs 1000 searches
                list.search(i * 1000 + j);
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    
    // Perform deletions for a long time (30 seconds)
    threads.clear();
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([&list, i]() {
            for (int j = 0; j < 1000; ++j) {  // Each thread performs 1000 deletions
                list.deleteNode(i * 1000 + j);
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Lock-Free List operations took: " << duration.count() << " seconds.\n";
}


int main() {
    int numThreads = 10;  // Number of threads performing operations
    std::cout << "Testing Linked List Performance..." << std::endl;
    // Lock-Based Test
    LockBasedLinkedList lockBasedList;
    testLockBasedList(lockBasedList, numThreads);
    
    // Lock-Free Test
    LinkedList lockFreeList;
    testLockFreeList(lockFreeList, numThreads);

    return 0;
}
