#include <atomic>
#include <vector>
#pragma once
class SPSCQueue
{
private:
    std::vector<int> m_buffer;
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
    size_t m_capacity;

public:
    SPSCQueue(size_t capacity);
    bool enqueue(int item);
    void dequeue(int &item);
};
