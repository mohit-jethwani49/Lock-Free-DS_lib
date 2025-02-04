#ifndef LOCK_BASED_LINKED_LIST_HPP
#define LOCK_BASED_LINKED_LIST_HPP

#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

struct Node {
    int key;
    void* value;
    std::shared_ptr<Node> next;

    Node(int k, void* v = nullptr) : key(k), value(v), next(nullptr) {}
};

// Lock-Based Linked List
class LockBasedLinkedList {
public:
    LockBasedLinkedList();
    bool insert(int k, void* value);
    bool deleteNode(int k);
    bool search(int key);
    void print() const;

private:
    std::shared_ptr<Node> head;
    mutable std::mutex list_mutex; // Mutex for thread safety

    // Helper function to find the node just before the target node
    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> searchFrom(int k);
};

LockBasedLinkedList::LockBasedLinkedList() {
    head = std::make_shared<Node>(std::numeric_limits<int>::min());
    head->next = std::make_shared<Node>(std::numeric_limits<int>::max());
}

std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> LockBasedLinkedList::searchFrom(int k) {
    auto curr = head;
    auto next = curr->next;

    while (next->key < k) {
        curr = next;
        next = curr->next;
    }

    return {curr, next};
}

bool LockBasedLinkedList::insert(int k, void* value) {
    std::lock_guard<std::mutex> lock(list_mutex); // Locking critical section

    auto [prev, next] = searchFrom(k);

    if (next->key == k) return false; // If the key exists, return false

    auto newNode = std::make_shared<Node>(k, value);
    newNode->next = next;

    prev->next = newNode; // Insert new node

    return true;
}

bool LockBasedLinkedList::deleteNode(int k) {
    std::lock_guard<std::mutex> lock(list_mutex); // Locking critical section

    auto [prev, delNode] = searchFrom(k);
    if (delNode->key != k) return false; // Node not found

    prev->next = delNode->next; // Remove the node from the list

    return true;
}

bool LockBasedLinkedList::search(int key) {
    std::lock_guard<std::mutex> lock(list_mutex); // Locking critical section

    auto [prev, next] = searchFrom(key);
    return next->key == key;
}

void LockBasedLinkedList::print() const {
    std::lock_guard<std::mutex> lock(list_mutex); // Locking critical section

    auto curr = head->next;
    while (curr->key != std::numeric_limits<int>::max()) {
        std::cout << curr->key << "\t";
        curr = curr->next;
    }
    std::cout << std::endl;
}



#endif