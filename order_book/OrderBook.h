#pragma once
#include <map>
#include <cstdint>
#include <deque>
#include <unordered_map>

enum class Side
{
    BUY,
    SELL
};

class Order
{
public:
    Order(double price, uint32_t quantity, Side side)
        : price(price), quantity(quantity), side(side) {}
    double price;
    uint64_t orderId;
    uint32_t quantity;
    Side side;
};

struct PriceLevels
{    
    double price;
    std::deque<Order> orders;
    int totalQuantity=0;
};

class OrderBook
{
public:
    void addOrder(uint64_t stockId, uint64_t timestamp, uint64_t orderId, Side side, double price, uint32_t quantity);
    void addOrderWithMPID(uint64_t stockId, uint64_t mpid, uint64_t timestamp, uint64_t orderId, Side side, double price, uint32_t quantity);
    void orderExecuted(uint64_t orderId, uint32_t executedQuantity, uint64_t matchingID);
    
    void removeOrder(Side side, uint64_t orderId);
    std::map<double, uint32_t> getOrders(Side side) const;

private:
    std::map<double, uint32_t> buyOrders;
    std::map<double, uint32_t> sellOrders;
    std::unordered_map<double, PriceLevels> buyPriceLevels;
    std::unordered_map<double, PriceLevels> sellPriceLevels;
};


