#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <cassert>
#include <type_traits>
#include <optional>
#include "Queue.hpp"


template <typename T, size_t size, size_t priority_count> class PriorityQueue {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(size > 2, "Buffer size must be bigger than 2");

    
  public:
    
    bool Push(const T &element, size_t priority);
    bool Pop(T &element);
    std::optional<T> PopOptional();

  private:
    Queue<T, size> _subqueue[priority_count];
};

template <typename T, size_t size, size_t priority_count>
bool PriorityQueue<T, size, priority_count>::Push(const T &element,
                                                  const size_t priority) {
    assert(priority < priority_count);

    return _subqueue[priority].Push(element);
}

template <typename T, size_t size, size_t priority_count>
bool PriorityQueue<T, size, priority_count>::Pop(T &element) {

    for (size_t priority = priority_count; priority-- > 0;) {
        if (_subqueue[priority].Pop(element)) {
            return true;
        }
    }

    return false;
}

#endif