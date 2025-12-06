#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include <string>

class OrderBook {
public:
    void addOrder(const std::string& order);
    void removeOrder(const std::string& order);
    std::vector<std::string> getOrders() const;

private:
    std::vector<std::string> orders;
};

#endif // ORDERBOOK_H