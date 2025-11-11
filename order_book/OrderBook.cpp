#include "OrderBook.h"

void OrderBook::addOrder(Side side, double price, uint32_t quantity)
{
    if (side == Side::BUY)
    {
        buyOrders[price] += quantity;
        buyPriceLevels[price].price = price;
        buyPriceLevels[price].orders.emplace_back(price, quantity, side);
        buyPriceLevels[price].totalQuantity += quantity;
    }
    else
    {
        sellOrders[price] += quantity;
        sellPriceLevels[price].price = price;
        sellPriceLevels[price].orders.emplace_back(price, quantity, side);
        sellPriceLevels[price].totalQuantity += quantity;
    }
}


void OrderBook::removeOrder(Side side, uint64_t orderId)
{
    if (side == Side::BUY)
    {
        for (auto& [price, level] : buyPriceLevels)
        {
            auto& orders = level.orders;
            for (auto it = orders.begin(); it != orders.end(); ++it)
            {
                if (it->orderId == orderId)
                {
                    level.totalQuantity -= it->quantity;
                    buyOrders[price] -= it->quantity;
                    orders.erase(it);
                    return;
                }
            }
        }
    }
    else
    {
        for (auto& [price, level] : sellPriceLevels)
        {
            auto& orders = level.orders;
            for (auto it = orders.begin(); it != orders.end(); ++it)
            {
                if (it->orderId == orderId)
                {
                    level.totalQuantity -= it->quantity;
                    sellOrders[price] -= it->quantity;
                    orders.erase(it);
                    return;
                }
            }
        }
    }
}

std::map<double, uint32_t> OrderBook::getOrders(Side side) const
{
    if (side == Side::BUY)
    {
        return buyOrders;
    }
    else
    {
        return sellOrders;
    }
}


