#include "orderbook.h"
#include "spscqueue.h"
#include <iostream>

int main() {
    // Create an instance of the OrderBook
    OrderBook orderBook;

    // Create an instance of the SPSCQueue
    SPSCQueue<int> queue;

    // Example usage of OrderBook
    orderBook.addOrder(1, "Buy", 100);
    orderBook.addOrder(2, "Sell", 150);
    
    // Example usage of SPSCQueue
    queue.enqueue(42);
    int item = queue.dequeue();

    // Output the results
    std::cout << "Dequeued item: " << item << std::endl;
    std::cout << "Current orders in the order book: " << std::endl;
    for (const auto& order : orderBook.getOrders()) {
        std::cout << "Order ID: " << order.id << ", Type: " << order.type << ", Price: " << order.price << std::endl;
    }

    return 0;
}