#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include "../include/Queue.hpp" // Include your lock-free queue

const int NUM_PRODUCERS = 4;
const int NUM_CONSUMERS = 4;
const int NUM_ITEMS = 10000000; // Total items produced by each producer

// Mutex-protected queue for comparison
std::queue<int> std_queue;
std::mutex queue_mutex;

void std_producer() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        std_queue.push(i);
    }
}

void std_consumer() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        int value;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (!std_queue.empty()) {
                value = std_queue.front();
                std_queue.pop();
            } else {
                --i; // Retry if queue is empty
            }
        }
        // std::cout << "Standard Consumer consumed: " << value << std::endl;
    }
}

void lockfree_producer(Queue<int, 10>& queue) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        while (!queue.Push(i)) {}
    }
}

void lockfree_consumer(Queue<int, 10>& queue) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        int value;
        while (!queue.Pop(value)) {}
        // std::cout << "Lock-Free Consumer consumed: " << value << std::endl;
    }
}

template <typename Func>
void measure_performance(Func f, const std::string& name) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << name << " took " << duration.count() << " seconds." << std::endl;
}

int main() {
    // Measure performance for standard queue
    std::cout << "Measuring standard queue performance..." << std::endl;
    measure_performance([] {
        std::vector<std::thread> producers, consumers;

        for (int i = 0; i < NUM_PRODUCERS; ++i) {
            producers.emplace_back(std_producer);
        }
        for (int i = 0; i < NUM_CONSUMERS; ++i) {
            consumers.emplace_back(std_consumer);
        }

        for (auto& p : producers) {
            p.join();
        }
        for (auto& c : consumers) {
            c.join();
        }
    }, "Standard Queue");

    // Measure performance for lock-free queue
    std::cout << "Measuring lock-free queue performance..." << std::endl;
    Queue<int, 10> lockfree_queue;
    measure_performance([&] {
        std::vector<std::thread> producers, consumers;

        for (int i = 0; i < NUM_PRODUCERS; ++i) {
            producers.emplace_back(lockfree_producer, std::ref(lockfree_queue));
        }
        for (int i = 0; i < NUM_CONSUMERS; ++i) {
            consumers.emplace_back(lockfree_consumer, std::ref(lockfree_queue));
        }

        for (auto& p : producers) {
            p.join();
        }
        for (auto& c : consumers) {
            c.join();
        }
    }, "Lock-Free Queue");

    return 0;
}
