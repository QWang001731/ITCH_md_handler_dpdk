#ifndef OrderBook_H
#define OrderBook_H

#include "spscqueue.h"
#include <vector>
#include <string>
#include <cstdint>
#include <atomic>
#include <map>
#include <memory>

using Price = int32_t;
using Qty = int32_t;
using OrderId = uint64_t;
#define KnuthMultiplier 11400714819323198485ull

enum class Side
{
    BUY = 0,
    SELL = 1,
};
class Order;
class PriceLevel;
class Slot
{
public:
    bool empty;
    bool tombstone;
    OrderId orderId;
    Order *order;
    Slot() : empty(true), tombstone(false), orderId(0), order(nullptr) {};
    Slot(uint64_t orderID) : empty(true), tombstone(false), orderId(orderID), order(nullptr) {}
};
class FlatMap
{
public:
    int32_t capacity;
    std::vector<Slot> slots;

    FlatMap()
    {
        capacity = 1 << 10; // default 1K entries
        slots.resize(capacity);
    }

    FlatMap(int32_t capacity) : capacity(capacity)
    {
        slots.resize(capacity);
    }

    inline int32_t hash(OrderId orderId)
    {
        return (orderId * KnuthMultiplier) & (capacity - 1);
    }
    bool insert(uint64_t orderId, Order *order);
    Order *find(uint64_t orderId);
    bool erase(uint64_t orderId);
};

class Order
{
public:
    uint64_t id;
    Side side;
    int32_t price;
    int32_t qty;
    Order *prev;
    Order *next;
    PriceLevel *level;

    Order(uint64_t id, Side side, int32_t price, int32_t qty)
        : id(id), side(side), price(price), qty(qty), prev(nullptr), next(nullptr), level(nullptr) {}

    ~Order()
    {
        // Clean up any resources if needed
    }
};
class PriceLevel
{
public:
    int32_t price;
    std::atomic<int32_t> total_qty;
    Order *head;
    Order *tail;

    PriceLevel(int32_t price) : price(price), total_qty(0), head(nullptr), tail(nullptr) {}
    ~PriceLevel()
    {
        // Clean up all orders in this price level
        Order *current = head;
        while (current)
        {
            Order *next = current->next;
            delete current;
            current = next;
        }
    }

    inline bool empty() const
    {
        return head == nullptr;
    }

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
            total_qty = o->qty;
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
};

// OrderBook for one instrument

class OrderBook
{
public:
    std::vector<PriceLevel *> bids; // map/flat_map/array
    std::vector<PriceLevel *> asks;
    int32_t min_price_cents{0};
    int32_t max_price_cents{0};
    FlatMap orders; // default 1K orders

    Price best_bid_price{};
    Qty best_bid_qty{};
    Price best_ask_price{};
    Qty best_ask_qty{};

    OrderBook(Price min_price_cents, Price max_price_cents)
        : min_price_cents(min_price_cents), max_price_cents(max_price_cents)
    {
        int32_t price_range = max_price_cents - min_price_cents + 1;
        bids.resize(price_range, nullptr);
        asks.resize(price_range, nullptr);
    }

    ~OrderBook()
    {
        // Clean up price levels
        for (auto &level : bids)
        {
            if (level)
            {
                delete level;
            }
        }

        for (auto &level : asks)
        {
            if (level)
            {
                delete level;
            }
        }
    }

    bool addOrder(uint64_t id, const std::string &side_str, int32_t price, int32_t qty);
    bool orderExecuted(uint64_t id, int32_t qty);
    bool orderCancel(uint64_t id, int32_t qty);
    bool orderCancelReplace(uint64_t oldId, uint64_t newId, int32_t newPrice, int32_t qty);
    bool orderDelete(uint64_t id);
    FlatMap &getOrders()
    {
        return orders;
    }
};

#endif // OrderBook_H
