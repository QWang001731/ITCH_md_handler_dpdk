#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include <string>
#include <cstdint>
#include <atomic>
#include <map>
#include "spscqueue.h"

using Price = int32_t;
using Qty = int32_t;
using OrderId = uint64_t;

enum class Side
{
    BUY = 0,
    SELL = 1,
};

class Order
{
    uint64_t id;
    Side side;
    int32_t price;
    int32_t qty;
    Order *prev;
    Order *next;
    PriceLevel *level = nullptr;

public:
    Order(uint64_t id, Side side, int32_t price, int32_t qty)
        : id(id), side(side), price(price), qty(qty), prev(nullptr), next(nullptr) {}
};

class PriceLevel
{
    int32_t price;
    std::atomic<int32_t> total_qty;
    Order *head;
    Order *tail;

public:
    PriceLevel(int32_t price) : price(price), total_qty(0), head(nullptr), tail(nullptr) {}
    void append(Order *o)
    {
        if (tail)
        {
            o->prev = tail;
            tail->next = o;
            tail = o;
            total_qty += o->qty;
            o->level = this;
            return;
        }
        else
        {
            head = o;
            tail = o;
            total_qty += o->qty;
            o->level = this;
            return;
        }
    }

    void remove(Order *o)
    {
        if (o->prev)
        {
            o->prev->next = o->next;
        }
        if (o->next)
        {
            o->next->prev = o->prev;
        }
        if (head == o)
        {
            head = o->next;
        }
        if (tail == o)
        {
            tail = o->prev;
        }
        total_qty -= o->qty;
        o->level = nullptr;
    }

    inline bool empty() const { return head == nullptr; }
};

// orderbook for one instrument
// Bids: highest price first
using BidLevels = std::map<Price, PriceLevel, std::greater<Price>>;
// Asks: lowest price first
using AskLevels = std::map<Price, PriceLevel, std::less<Price>>;
class OrderBook
{
private:
    BidLevels bids; // map/flat_map/array
    AskLevels asks;
    std::unordered_map<OrderId, Order *> orders;

public:
    Price best_bid_price;
    Qty best_bid_qty;
    Price best_ask_price;
    Qty best_ask_qty;
    OrderBook();
    void addOrder(uint64_t id, const std::string &side_str, int32_t price, int32_t qty);
    void orderExecuted(uint64_t id, int32_t qty);
    void orderCancel(uint64_t id, int32_t qty);
    void orderCancelReplace(uint64_t oldId, uint64_t newId, int32_t newPrice, int32_t qty);
    void orderDelete(uint64_t id);
    const std::unordered_map<OrderId, Order *> &getOrders() const { return orders; };
};

#endif // ORDERBOOK_H