#include "OrderBook.h"
#include <algorithm>

OrderBook::OrderBook() {}

// Updated to use shared_ptr for orders and price levels consistently
void OrderBook::addOrder(uint64_t id, const std::string &side_str, int32_t price, int32_t qty)
{
    Side side = (side_str == "Buy") ? Side::BUY : Side::SELL;
    orders[id] = new Order(id, side, price, qty);
    int32_t idx = (price - min_price_cents);
    if (side == Side::BUY)
    {
        if (bids[idx] == nullptr)
        {
            bids[idx] = new PriceLevel(price);
        }
        bids[idx]->append(orders[id]);
        if (best_bid_price < price)
        {
            best_bid_price = price;
            best_bid_qty = bids[idx]->total_qty.load();
        }
    }
    else
    {
        if (asks[idx] == nullptr)
        {
            asks[idx] = new PriceLevel(price);
        }
        asks[idx]->append(orders[id]);
        if (best_ask_price > price || best_ask_price == 0)
        {
            best_ask_price = price;
            best_ask_qty = asks[idx]->total_qty.load();
        }
    }
}

void OrderBook::orderExecuted(uint64_t id, int32_t qty)
{
    auto it = orders.find(id);
    if (it != orders.end())
    {
        auto order = it->second; // shared_ptr<Order>
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

void OrderBook::orderCancel(uint64_t id, int32_t qty)
{
    auto it = orders.find(id);
    if (it != orders.end())
    {
        auto order = it->second; // shared_ptr<Order>
        if (order->qty > qty)
        {
            order->qty -= qty;
            if (order->level)
            {
                order->level->total_qty -= qty;
            }
        }
        else
        {
            // remove whole order
            if (order->level)
            {
                order->level->remove(order.get());
            }
            orders.erase(it);
        }
    }
}

void OrderBook::orderCancelReplace(uint64_t oldId, uint64_t newId, int32_t newPrice, int32_t qty)
{
    auto it = orders.find(oldId);
    if (it != orders.end())
    {
        auto oldOrder = it->second; // shared_ptr<Order>
        Side side = oldOrder->side;

        // Remove old order from its price level (if any)
        if (oldOrder->level)
        {
            oldOrder->level->remove(oldOrder.get());
        }

        // erase old order
        orders.erase(it);

        // Add new order
        auto newOrder = std::make_shared<Order>(newId, side, newPrice, qty);
        orders[newId] = newOrder;

        if (side == Side::BUY)
        {
            if (bids.find(newPrice) == bids.end())
            {
                bids[newPrice] = std::make_shared<PriceLevel>(newPrice);
            }
            bids[newPrice]->append(newOrder.get());
        }
        else
        {
            if (asks.find(newPrice) == asks.end())
            {
                asks[newPrice] = std::make_shared<PriceLevel>(newPrice);
            }
            asks[newPrice]->append(newOrder.get());
        }
    }
}

void OrderBook::orderDelete(uint64_t id)
{
    auto it = orders.find(id);
    if (it != orders.end())
    {
        auto order = it->second; // shared_ptr<Order>
        if (order->level)
        {
            order->level->remove(order.get());
        }
        orders.erase(it);
    }
}

void OrderBook::trade_non_cross(uint64_t orderId, int32_t price, int32_t qty)
{
    auto it = orders.find(orderId);
    if (it != orders.end())
    {
        auto order = it->second; // shared_ptr<Order>
        if (order->qty > qty)
        {
            order->qty -= qty;
            if (order->level)
            {
                order->level->total_qty -= qty;
            }
        }
        else if (order->qty == qty)
        {
            // remove whole order
            if (order->level)
            {
                order->level->remove(order.get());
            }
            orders.erase(it);
        }
    }
}
