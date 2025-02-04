#ifndef RING_BUF_HPP
#define RING_BUF_HPP

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <iostream>
#include <vector>
#include <mutex>
#include <optional>
#include <stdlib.h>
#include <condition_variable>

#include <mutex>
#include <queue>

template <typename T>
class LockBasedBuffer {
public:
    explicit LockBasedBuffer(size_t max_size) : max_size_(max_size) {}

    bool Write(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() < max_size_) {
            queue_.push(item);
            return true;
        }
        return false;
    }

    bool Read(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!queue_.empty()) {
            item = queue_.front();
            queue_.pop();
            return true;
        }
        return false;
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<T> queue_;
    size_t max_size_;
    mutable std::mutex mutex_;
};


template <typename T, size_t size> class RingBuf {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(size > 2, "Buffer size must be bigger than 2");

  public:
    RingBuf();

    bool Write(const T *data, size_t cnt);


    template <size_t arr_size> bool Write(const std::array<T, arr_size> &data);
    bool Read(T *data, size_t cnt);
    template <size_t arr_size> bool Read(std::array<T, arr_size> &data);
    bool Peek(T *data, size_t cnt) const;
    template <size_t arr_size> bool Peek(std::array<T, arr_size> &data) const;
    bool Skip(size_t cnt);
    size_t GetFree() const;
    size_t GetAvailable() const;

  private:
    static size_t CalcFree(const size_t w, const size_t r);
    static size_t CalcAvailable(const size_t w, const size_t r);


  private:
    T _data[size];

    std::atomic_size_t _r; 
    std::atomic_size_t _w; 
};

template <typename T, size_t size>
RingBuf<T, size>::RingBuf() : _r(0U), _w(0U) {}

template <typename T, size_t size>
bool RingBuf<T, size>::Write(const T *data, const size_t cnt) {
    
    size_t w = _w.load(std::memory_order_relaxed);
    const size_t r = _r.load(std::memory_order_acquire);

    if (CalcFree(w, r) < cnt) {
        return false;
    }
    if (w + cnt <= size) {
        memcpy(&_data[w], &data[0], cnt * sizeof(T));
        w += cnt;
        if (w == size) {
            w = 0U;
        }
    } else {

        const size_t linear_free = size - w;
        memcpy(&_data[w], &data[0], linear_free * sizeof(T));
        const size_t remaining = cnt - linear_free;
        memcpy(&_data[0], &data[linear_free], remaining * sizeof(T));

        w = remaining;
    }

    _w.store(w, std::memory_order_release);

    return true;
}

template <typename T, size_t size>
bool RingBuf<T, size>::Read(T *data, const size_t cnt) {
    size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    if (CalcAvailable(w, r) < cnt) {
        return false;
    }
    if (r + cnt <= size) {
        memcpy(&data[0], &_data[r], cnt * sizeof(T));
        
        r += cnt;
        if (r == size) {
            r = 0U;
        }
    } else {
        
        const size_t linear_available = size - r;
        memcpy(&data[0], &_data[r], linear_available * sizeof(T));
        
        const size_t remaining = cnt - linear_available;
        memcpy(&data[linear_available], &_data[0], remaining * sizeof(T));
        
        r = remaining;
    }

    
    _r.store(r, std::memory_order_release);

    return true;
}

template <typename T, size_t size>
bool RingBuf<T, size>::Peek(T *data, const size_t cnt) const {
    const size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    if (CalcAvailable(w, r) < cnt) {
        return false;
    }
    if (r + cnt <= size) {
        memcpy(&data[0], &_data[r], cnt * sizeof(T));
    } else {
        const size_t linear_available = size - r;
        memcpy(&data[0], &_data[r], linear_available * sizeof(T));
        const size_t remaining = cnt - linear_available;
        memcpy(&data[linear_available], &_data[0], remaining * sizeof(T));
    }

    return true;
}

template <typename T, size_t size>
bool RingBuf<T, size>::Skip(const size_t cnt) {
    size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    if (CalcAvailable(w, r) < cnt) {
        return false;
    }

    r += cnt;
    if (r >= size) {
        r -= size;
    }
    _r.store(r, std::memory_order_release);

    return true;
}

template <typename T, size_t size> size_t RingBuf<T, size>::GetFree() const {
    const size_t w = _w.load(std::memory_order_relaxed);
    const size_t r = _r.load(std::memory_order_acquire);

    return CalcFree(w, r);
}

template <typename T, size_t size>
size_t RingBuf<T, size>::GetAvailable() const {
    const size_t r = _r.load(std::memory_order_relaxed);
    const size_t w = _w.load(std::memory_order_acquire);

    return CalcAvailable(w, r);
}


template <typename T, size_t size>
template <size_t arr_size>
bool RingBuf<T, size>::Write(const std::array<T, arr_size> &data) {
    return Write(data.begin(), arr_size);
}

template <typename T, size_t size>
template <size_t arr_size>
bool RingBuf<T, size>::Read(std::array<T, arr_size> &data) {
    return Read(data.begin(), arr_size);
}

template <typename T, size_t size>
template <size_t arr_size>
bool RingBuf<T, size>::Peek(std::array<T, arr_size> &data) const {
    return Peek(data.begin(), arr_size);
}


template <typename T, size_t size>
size_t RingBuf<T, size>::CalcFree(const size_t w, const size_t r) {
    if (r > w) {
        return (r - w) - 1U;
    } else {
        return (size - (w - r)) - 1U;
    }
}

template <typename T, size_t size>
size_t RingBuf<T, size>::CalcAvailable(const size_t w, const size_t r) {
    if (w >= r) {
        return w - r;
    } else {
        return size - (r - w);
    }
}

#endif 
