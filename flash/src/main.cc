#include "orderbook.h"
#include "spscqueue.h"
#include <iostream>

int main()
{
    std::cout << "Creating OrderBook..." << std::endl;
    OrderBook orderBook(0, 10000);

    std::cout << "Adding order..." << std::endl;
    bool success = orderBook.addOrder(1, "Buy", 100, 10);

    std::cout << "Result: " << (success ? "SUCCESS" : "FAILED") << std::endl;

    return 0;
}