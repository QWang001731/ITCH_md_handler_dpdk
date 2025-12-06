#include "orderbook.h"
#include <vector>
#include <algorithm>

OrderBook::OrderBook() {}

void OrderBook::addOrder(uint64_t id, const std::string &side_str, int32_t price, int32_t qty)
{
    Side side = (side_str == "Buy") ? Side::BUY : Side::SELL;
    Order newOrder(id, side, price, qty);
    orders[id] = &newOrder;
    if (side == Side::BUY)
    {
        if (bids.find(price) == bids.end())
        {
            bids[price] = new PriceLevel(price);
        }
        bids[price]->append(&newOrder);
        if (best_bid_price < price)
        {
            best_bid_price = price;
            best_bid_qty = bids[price]->total_qty.load();
        }
    }
    else
    {
        if (asks.find(price) == asks.end())
        {
            asks[price] = new PriceLevel(price);
        }
        asks[price]->append(&newOrder);
        if (best_ask_price > price || best_ask_price == 0)
        {
            best_ask_price = price;
            best_ask_qty = asks[price]->total_qty.load();
        }
    }
}

void orderBook::orderExecuted(uint64_t id, int32_t qty)
{
    auto it = orders.find(id);
    if (it != orders.end())
    {
        Order *order = it->second;
        if (order->qty >= qty)
        {
            order->qty -= qty;
            if (order->level)
            {
                order->level->total_qty -= qty;
            }
        }
    }
}

void orderBook::removeOrder(uint64_t id)
{
    auto it = orders.find(id);
    if (it != orders.end())
    {
        Order *order = it->second;
        PriceLevel *level = order->level;
        if (level)
        {
            // Update total quantity
            level->total_qty -= order->qty;
            // Remove order from linked list
            if (order->prev)
            {
                order->prev->next = order->next;
            }
            if (order->next)
            {
                order->next->prev = order->prev;
            }
            if (level->head == order)
            {
                level->head = order->next;
            }
            if (level->tail == order)
            {
                level->tail = order->prev;
            }
        }
        orders.erase(it);
    }
}
