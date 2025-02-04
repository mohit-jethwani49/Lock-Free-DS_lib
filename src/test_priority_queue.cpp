#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include "../include/Priority_Queue.hpp" // Include your lock-free priority queue

const int NUM_PRODUCERS = 4;
const int NUM_CONSUMERS = 4;
const int NUM_ITEMS = 10000000;
const int PRIORITY_COUNT = 4;


std::priority_queue<std::pair<int, int>> std_priority_queue;
std::mutex priority_queue_mutex;

std::atomic<int> items_produced(0);
std::atomic<int> items_consumed(0);
std::atomic<bool> producers_done{false};

void std_priority_producer(int priority) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        {
            std::lock_guard<std::mutex> lock(priority_queue_mutex);
            std_priority_queue.emplace(priority, i);
        }
        items_produced.fetch_add(1);
    }

    if (items_produced.load() == NUM_ITEMS * NUM_PRODUCERS) {
        producers_done.store(true);        
    }
}

void std_priority_consumer() {
    while (true) {
        int value = -1;
        {
            std::lock_guard<std::mutex> lock(priority_queue_mutex);

            if (!std_priority_queue.empty()) {
                value = std_priority_queue.top().second;
                std_priority_queue.pop();
                items_consumed.fetch_add(1);
            }
        }

        
        if (value == -1 && producers_done.load() && std_priority_queue.empty()) {
            break;
        }

    }
}


void lockfree_priority_producer(PriorityQueue<int, 10, PRIORITY_COUNT>& queue, int priority) {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        while (!queue.Push(i, priority)) {}
    }
}

void lockfree_priority_consumer(PriorityQueue<int, 10, PRIORITY_COUNT>& queue) {
    while (items_consumed.load() < NUM_ITEMS * NUM_PRODUCERS) {
        int value;
        if (queue.Pop(value)) {
            items_consumed.fetch_add(1);
        }
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
    
    std::cout << "Measuring standard priority queue performance..." << std::endl;
    measure_performance([] {
        std::vector<std::thread> producers, consumers;

        for (int priority = 0; priority < PRIORITY_COUNT; ++priority) {
            for (int i = 0; i < NUM_PRODUCERS / PRIORITY_COUNT; ++i) {
                producers.emplace_back(std_priority_producer, priority);
            }
        }
        for (int i = 0; i < NUM_CONSUMERS; ++i) {
            consumers.emplace_back(std_priority_consumer);
        }
        // std::thread progress_logger([] {

        for (auto& p : producers) {
            p.join();
        }
        for (auto& c : consumers) {
            c.join();
        }
    }, "Standard Priority Queue");

    
    items_produced = 0;
    items_consumed = 0;

    std::cout << "Measuring lock-free priority queue performance..." << std::endl;
    PriorityQueue<int, 10, PRIORITY_COUNT> lockfree_priority_queue;
    measure_performance([&] {
        std::vector<std::thread> producers, consumers;

        for (int priority = 0; priority < PRIORITY_COUNT; ++priority) {
            for (int i = 0; i < NUM_PRODUCERS / PRIORITY_COUNT; ++i) {
                producers.emplace_back(lockfree_priority_producer, std::ref(lockfree_priority_queue), priority);
            }
        }
        for (int i = 0; i < NUM_CONSUMERS; ++i) {
            consumers.emplace_back(lockfree_priority_consumer, std::ref(lockfree_priority_queue));
        }

        for (auto& p : producers) {
            p.join();
        }
        for (auto& c : consumers) {
            c.join();
        }

    }, "Lock-Free Priority Queue");

    return 0;
}
