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

void orderBook::orderCanceled(uint64_t id, int32_t qty)
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

void orderBook::orderCancel(uint64_t id, int32_t qty)
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

void orderBook::orderCancelReplace(uint64_t oldId, uint64_t newId, int32_t newPrice, int32_t qty)
{
    auto it = orders.find(oldId);
    if (it != orders.end())
    {
        Order *oldOrder = it->second;
        Side side = oldOrder->side;
        // Remove old order
        if (oldOrder->level)
        {
            oldOrder->level->total_qty -= oldOrder->qty;
            // Remove from linked list
            if (oldOrder->prev)
            {
                oldOrder->prev->next = oldOrder->next;
            }
            if (oldOrder->next)
            {
                oldOrder->next->prev = oldOrder->prev;
            }
            if (oldOrder->level->head == oldOrder)
            {
                oldOrder->level->head = oldOrder->next;
            }
            if (oldOrder->level->tail == oldOrder)
            {
                oldOrder->level->tail = oldOrder->prev;
            }
        }
        orders.erase(it);

        // Add new order
        Order *newOrder = new Order(newId, side, newPrice, qty);
        orders[newId] = newOrder;
        if (side == Side::BUY)
        {
            if (bids.find(newPrice) == bids.end())
            {
                bids[newPrice] = new PriceLevel(newPrice);
            }
            bids[newPrice]->append(newOrder);
        }
        else
        {
            if (asks.find(newPrice) == asks.end())
            {
                asks[newPrice] = new PriceLevel(newPrice);
            }
            asks[newPrice]->append(newOrder);
        }
    }
}

void orderBook::orderDelete(uint64_t id)
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
