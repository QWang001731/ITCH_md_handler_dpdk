#include <MPMCQueue.hpp>
MPMCQueue::MPMCQueue(/* args */)
    : m_capacity(1024), m_buffer(1024), m_head(0), m_tail(0)
{
}

MPMCQueue::~MPMCQueue()
{
}
bool MPMCQueue::enqueue(int item)
{
    size_t tail, next_tail;

    while (true)
    {
        tail = m_tail.load(std::memory_order_relaxed);
        next_tail = (tail + 1) % m_capacity;

        if (next_tail == m_head.load(std::memory_order_relaxed))
        {
            return false; // Queue is full
        }

        if (m_tail.compare_exchange_weak(tail, next_tail))
        {
            m_buffer[next_tail] = item;
            break;
        }
    }

    return true;
}