#include <atomic>
#include <vector>

#pragma once
class MPMCQueue
{
private:
    /* data */
    size_t m_capacity;
    std ::vector<int> m_buffer;
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;

public:
    MPMCQueue(/* args */);
    ~MPMCQueue();

    bool enqueue(int item);
    bool dequeue(int &item);
};