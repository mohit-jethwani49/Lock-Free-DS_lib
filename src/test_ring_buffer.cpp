
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "../include/RingBuffer.hpp"
// Include the RingBuf code you provided here.

void testLockFreeBuffer() {
    constexpr size_t buffer_size = 1024;
    constexpr size_t num_producers = 4;
    constexpr size_t num_consumers = 4;
    constexpr size_t items_per_producer = 10000000;

    RingBuf<int, buffer_size> buffer;
    std::atomic<size_t> items_produced{0}, items_consumed{0};
    std::atomic<bool> producers_done{false};

    auto producer = [&]() {
    for (int i = 0; i < items_per_producer; ++i) {
        while (!buffer.Write(&i, 1)) {
        }
        ++items_produced;
    }
};


    auto consumer = [&]() {
        int value;
        while (true) {
            if (buffer.Read(&value, 1)) {
                ++items_consumed;
            } else if (producers_done.load()) {
                break;
            }
        }
    };

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> producers, consumers;

    for (size_t i = 0; i < num_producers; ++i) {
        producers.emplace_back(producer);
    }
    for (size_t i = 0; i < num_consumers; ++i) {
        consumers.emplace_back(consumer);
    }

    for (auto& p : producers) {
        p.join();
    }
    producers_done.store(true);

    for (auto& c : consumers) {
        c.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Lock-Free Ring Buffer:" << std::endl;
    std::cout << "Elapsed Time: " << elapsed.count() << " seconds\n" << std::endl;
}

void testLockBasedBuffer() {
    constexpr size_t buffer_size = 1024;
    constexpr size_t num_producers = 4;
    constexpr size_t num_consumers = 4;
    constexpr size_t items_per_producer = 10000000;

    LockBasedBuffer<int> buffer(buffer_size);
    std::atomic<size_t> items_produced{0}, items_consumed{0};
    std::atomic<bool> producers_done{false};

    auto producer = [&]() {
        for (size_t i = 0; i < items_per_producer; ++i) {
            while (!buffer.Write(i)) {
                // Spin if buffer is full.
            }
            ++items_produced;
        }
    };

    auto consumer = [&]() {
        int value;
        while (true) {
            if (buffer.Read(value)) {
                ++items_consumed;
            } else if (producers_done.load()) {
                break;
            }
        }
    };

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> producers, consumers;

    for (size_t i = 0; i < num_producers; ++i) {
        producers.emplace_back(producer);
    }
    for (size_t i = 0; i < num_consumers; ++i) {
        consumers.emplace_back(consumer);
    }

    for (auto& p : producers) {
        p.join();
    }
    producers_done.store(true);

    for (auto& c : consumers) {
        c.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Lock-Based Ring Buffer:" << std::endl;
    std::cout << "Elapsed Time: " << elapsed.count() << " seconds\n" << std::endl;
}

int main() {
    std::cout << "Testing Lock-Based Ring Buffer...\n";
    testLockBasedBuffer();

    std::cout << "Testing Lock-Free Ring Buffer...\n";
    testLockFreeBuffer();


    return 0;
}
