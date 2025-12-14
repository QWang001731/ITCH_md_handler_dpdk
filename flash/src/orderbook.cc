#include "orderbook.h"
#include <algorithm>
#include <iostream>

// Updated to use shared_ptr for orders and price levels consistently
bool OrderBook::addOrder(uint64_t id, const std::string &side_str, int32_t price, int32_t qty)
{
    std::cout << "addOrder called: id=" << id << " price=" << price << std::endl;

    if (price < min_price_cents || price > max_price_cents)
    {
        std::cout << "Price out of range" << std::endl;
        return false;
    }

    Side side = (side_str == "Buy") ? Side::BUY : Side::SELL;
    Order *newOrder = new Order(id, side, price, qty);
    std::cout << "Created order, calling insert..." << std::endl;

    if (!orders.insert(id, newOrder))
    {
        std::cout << "Insert failed" << std::endl;
        delete newOrder;
        return false;
    }

    int32_t idx = (price - min_price_cents);
    if (side == Side::BUY)
    {
        if (bids[idx] == nullptr)
        {
            bids[idx] = new PriceLevel(price);
        }
        bids[idx]->append(orders.find(id));
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
        asks[idx]->append(orders.find(id));
        if (best_ask_price > price || best_ask_price == 0)
        {
            best_ask_price = price;
            best_ask_qty = asks[idx]->total_qty.load();
        }
    }

    return true;
}

bool OrderBook::orderExecuted(uint64_t id, int32_t qty)
{
    Order *order = orders.find(id);
    if (order)
    {
        if (order->qty > qty)
        {
            order->qty -= qty;
            if (order->level)
            {
                order->level->total_qty -= qty;
            }
            return true;
        }
        else
        {
            // remove whole order
            if (order->level)
            {
                order->level->remove(order);
            }
            orders.erase(id);
            return true;
        }
    }
    return false;
}

// a portion of an order is canceled
bool OrderBook::orderCancel(uint64_t id, int32_t qty)
{
    Order *order = orders.find(id);
    if (order)
    {
        if (order->qty > qty)
        {
            order->qty -= qty;
            if (order->level)
            {
                order->level->total_qty -= qty;
            }
            return true;
        }
        else
        {
            // remove whole order
            if (order->level)
            {
                order->level->remove(order);
            }
            orders.erase(id);
            return true;
        }
    }
    return false;
}

bool OrderBook::orderDelete(uint64_t id)
{
    Order *order = orders.find(id);
    if (order)
    {
        if (order->level)
        {
            order->level->remove(order);
        }
        orders.erase(id);
        return true;
    }
    return false;
}

bool OrderBook::orderCancelReplace(uint64_t oldId, uint64_t newId, int32_t newPrice, int32_t qty)
{
    Order *order = orders.find(oldId);
    if (order)
    {
        Side side = order->side;

        // Remove old order from its price level (if any)
        if (order->level)
        {
            order->level->remove(order);
        }

        // erase old order
        orders.erase(oldId);

        // Add new order
        orders.insert(newId, new Order(newId, side, newPrice, qty));
        uint32_t idx = (newPrice - min_price_cents);

        if (side == Side::BUY)
        {
            if (bids[idx] == nullptr)
            {
                bids[idx] = new PriceLevel(newPrice);
            }
            bids[idx]->append(orders.find(newId));
        }
        else
        {
            if (asks[idx] == nullptr)
            {
                asks[idx] = new PriceLevel(newPrice);
            }
            asks[idx]->append(orders.find(newId));
        }
        return true;
    }

    return false;
}
bool FlatMap::insert(uint64_t orderId, Order *order)
{
    int32_t idx = hash(orderId);
    for (int32_t i = 0; i < capacity; ++i)
    {
        int32_t probeIdx = (idx + i) % capacity;
        Slot &slot = slots[probeIdx];
        if (slot.empty || slot.tombstone)
        {
            slot.empty = false;
            slot.tombstone = false;
            slot.orderId = orderId;
            slot.order = order;
            return true;
        }
    }
    return false; // map full
};

Order *FlatMap::find(uint64_t orderId)
{
    int32_t idx = hash(orderId);
    for (int32_t i = 0; i < capacity; ++i)
    {
        int32_t probeIdx = (idx + i) % capacity;
        Slot &slot = slots[probeIdx];

        if (!slot.empty && !slot.tombstone && orderId == slot.orderId)
        {
            return slot.order;
        }

        if (slot.empty && !slot.tombstone)
        {
            break; // not found
        }
    }

    return nullptr; // not found
};

bool FlatMap::erase(uint64_t orderId)
{
    int32_t idx = hash(orderId);
    for (int32_t i = 0; i < capacity; ++i)
    {
        int32_t probeIdx = (idx + i) % capacity;
        Slot &slot = slots[probeIdx];

        if (!slot.empty && !slot.tombstone && orderId == slot.orderId)
        {
            slot.empty = true;
            slot.tombstone = true;
            delete slot.order;
            slot.order = nullptr;
            return true;
        }

        if (slot.empty && !slot.tombstone)
        {
            break; // not found
        }
    }

    return false; // not found
};