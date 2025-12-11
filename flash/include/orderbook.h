#ifndef OrderBook_H
#define OrderBook_H

#include <vector>
#include <string>
#include <cstdint>
#include <atomic>
#include <map>
#include <unordered_map>
#include "Spscqueue.h"
#include <memory>

using Price = int32_t;
using Qty = int32_t;
using OrderId = uint64_t;

enum class Side
{
    BUY = 0,
    SELL = 1,
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

class Order
{
public:
    uint64_t id;
    Side side;
    int32_t price;
    int32_t qty;
    Order *prev;
    Order *next;
    std::shared_ptr<PriceLevel> level{nullptr};

    Order(uint64_t id, Side side, int32_t price, int32_t qty)
        : id(id), side(side), price(price), qty(qty), prev(nullptr), next(nullptr) {}
};

// OrderBook for one instrument

class OrderBook
{
private:
    std::vector<PriceLevel *> bids; // map/flat_map/array
    std::vector<PriceLevel *> asks;
    int32 min_price_cents{0};
    int32 max_price_cents{0};
    std::unordered_map<OrderId, std::shared_ptr<Order>> orders;

public:
    Price best_bid_price{};
    Qty best_bid_qty{};
    Price best_ask_price{};
    Qty best_ask_qty{};
    OrderBook() = 0;
    OrderBook(int32_t min_price_cents, int32_t max_price_cents)
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

    void addOrder(uint64_t id, const std::string &side_str, int32_t price, int32_t qty);
    void orderExecuted(uint64_t id, int32_t qty);
    void orderCancel(uint64_t id, int32_t qty);
    void orderCancelReplace(uint64_t oldId, uint64_t newId, int32_t newPrice, int32_t qty);
    void orderDelete(uint64_t id);
    void trade_non_cross(uint64_t orderId, int32_t price, int32_t qty);
    const std::unordered_map<OrderId, std::shared_ptr<Order>> &getOrders() const { return orders; };
};

#endif // OrderBook_H
