// This file declares the SPSCQueue class and its public methods, including enqueue and dequeue.

#ifndef SPSCQUEUE_H
#define SPSCQUEUE_H

#include <atomic>
#include <cstddef>
#include <vector>

template <typename T>
class SPSCQueue
{
public:
    explicit SPSCQueue(size_t capacity);
    ~SPSCQueue();

    bool enqueue(const T &item);
    bool dequeue(T &item);

private:
    std::vector<T> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    const size_t capacity;
};

#endif // SPSCQUEUE_H