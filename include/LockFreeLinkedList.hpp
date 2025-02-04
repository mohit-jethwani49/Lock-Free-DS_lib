#ifndef LOCK_FREE_LINKED_LIST_HPP
#define LOCK_FREE_LINKED_LIST_HPP

#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>



class LinkedList {
public:
    LinkedList();
    std::shared_ptr<Node> search(int key);
    bool insert(int k, void* value);
    bool deleteNode(int k);
    void print() const;
    void destroy();

private:
    std::shared_ptr<Node> head;
    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> searchFrom(int k);
};

LinkedList::LinkedList() {
    head = std::make_shared<Node>(std::numeric_limits<int>::min());
    head->next = std::make_shared<Node>(std::numeric_limits<int>::max());
}

std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> LinkedList::searchFrom(int k) {
    auto curr = head;
    auto next = curr->next;

    // Traverse the list, check for nullptr before accessing next->key
    while (next && next->key <= k) {
        if (next->key == k) return {curr, next};
        curr = next;
        next = curr->next;
    }

    return {curr, next};
}

bool LinkedList::insert(int k, void* value) {
    auto [prev, next] = searchFrom(k);

    // Ensure next is not null before dereferencing
    if (next && next->key == k) return false;

    auto newNode = std::make_shared<Node>(k, value);
    newNode->next = next;

    if (std::atomic_compare_exchange_strong(&prev->next, &next, newNode)) {
        return true;
    }
    return false;
}

bool LinkedList::deleteNode(int k) {
    auto [prev, delNode] = searchFrom(k);

    // Ensure delNode is not null before dereferencing
    if (!delNode || delNode->key != k) return false;

    if (std::atomic_compare_exchange_strong(&prev->next, &delNode, delNode->next)) {
        return true;
    }
    return false;
}


void LinkedList::print() const {
    auto curr = head->next;
    while (curr->key != std::numeric_limits<int>::max()) {
        std::cout << curr->key << "\t";
        curr = curr->next;
    }
    std::cout << std::endl;
}

void LinkedList::destroy() {
    auto curr = head;
    while (curr) {
        curr = curr->next;
    }
}

std::shared_ptr<Node> LinkedList::search(int key) {
    std::shared_ptr<Node> current = head;  // Start with the head of the list

    while (current != nullptr) {
        if (current->key == key) {
            return current;  // Return the node if found
        }
        current = current->next;  // Move to the next node
    }

    return nullptr;  // Return nullptr if key is not found
}


#endif