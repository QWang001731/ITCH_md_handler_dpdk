Here are the contents for the file /orderbook-project/orderbook-project/src/spscqueue.cc:

#include "spscqueue.h"

template <typename T>
SPSCQueue<T>::SPSCQueue(size_t capacity) : capacity(capacity), head(0), tail(0) {
    buffer = new T[capacity];
}

template <typename T>
SPSCQueue<T>::~SPSCQueue() {
    delete[] buffer;
}

template <typename T>
bool SPSCQueue<T>::enqueue(const T& item) {
    size_t next = (head + 1) % capacity;
    if (next == tail) {
        return false; // Queue is full
    }
    buffer[head] = item;
    head = next;
    return true;
}

template <typename T>
bool SPSCQueue<T>::dequeue(T& item) {
    if (head == tail) {
        return false; // Queue is empty
    }
    item = buffer[tail];
    tail = (tail + 1) % capacity;
    return true;
}

// Explicit instantiation for commonly used types
template class SPSCQueue<int>;
template class SPSCQueue<double>;
template class SPSCQueue<std::string>;