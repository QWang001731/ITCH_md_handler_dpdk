#include "orderbook.h"
#include <vector>
#include <algorithm>

OrderBook::OrderBook() {}

void OrderBook::addOrder(const Order& order) {
    orders.push_back(order);
}

void OrderBook::removeOrder(int orderId) {
    orders.erase(std::remove_if(orders.begin(), orders.end(),
                                  [orderId](const Order& order) {
                                      return order.id == orderId;
                                  }), orders.end());
}

std::vector<Order> OrderBook::getOrders() const {
    return orders;
}