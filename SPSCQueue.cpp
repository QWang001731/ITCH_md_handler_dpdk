#include "SPSCQueue.hpp"
SPSCQueue::SPSCQueue(size_t capacity)
    : m_buffer(capacity), m_head(0), m_tail(0), m_capacity(capacity)
{
}

bool SPSCQueue::enqueue(int item)
{
    if ((m_tail.load(std::memory_order_relaxed) + 1) % m_capacity == m_head.load(std::memory_order_relaxed))
    {
        return false;
    }
    m_buffer[m_tail.load(std::memory_order_relaxed)] = item;
    m_tail.store((m_tail.load(std::memory_order_relaxed) + 1) % m_capacity, std::memory_order_relaxed);
    return true;
}

void SPSCQueue::dequeue(int &item)
{
    if (m_head.load(std::memory_order_relaxed) == m_tail.load(std::memory_order_relaxed))
    {
        item = -1;
        return;
    }
    item = m_buffer[m_head.load(std::memory_order_relaxed)];
    m_head.store((m_head.load(std::memory_order_relaxed) + 1) % m_capacity, std::memory_order_relaxed);
    return;
}
