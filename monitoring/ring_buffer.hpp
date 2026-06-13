#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

template<typename T, size_t Size>
class RingBuffer {
private:
    std::vector<T> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    mutable std::mutex mutex;
    std::condition_variable cv;
    
public:
    RingBuffer() : buffer(Size), head(0), tail(0) {}
    
    bool push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex);
        size_t next_head = (head.load() + 1) % Size;
        if (next_head == tail.load()) {
            return false; // Buffer full
        }
        buffer[head.load()] = item;
        head.store(next_head);
        cv.notify_one();
        return true;
    }
    
    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        if (tail.load() == head.load()) {
            return false; // Buffer empty
        }
        item = buffer[tail.load()];
        tail.store((tail.load() + 1) % Size);
        return true;
    }
    
    bool peek(T& item) const {
        std::lock_guard<std::mutex> lock(mutex);
        if (tail.load() == head.load()) {
            return false;
        }
        item = buffer[tail.load()];
        return true;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (head.load() >= tail.load()) {
            return head.load() - tail.load();
        }
        return Size - tail.load() + head.load();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return head.load() == tail.load();
    }
    
    bool full() const {
        std::lock_guard<std::mutex> lock(mutex);
        return (head.load() + 1) % Size == tail.load();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        head.store(0);
        tail.store(0);
    }
    
    std::vector<T> snapshot() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<T> result;
        size_t current = tail.load();
        while (current != head.load()) {
            result.push_back(buffer[current]);
            current = (current + 1) % Size;
        }
        return result;
    }
};

#endif
