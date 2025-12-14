#include <gtest/gtest.h>
#include "orderbook.h"

class OrderBookTest : public ::testing::Test
{
protected:
    OrderBook orderBook{0, 10000}; // Initialize with price range

    void SetUp() override
    {
        // Setup code if needed
    }

    void TearDown() override
    {
        // Cleanup code if needed
    }
};

// Test adding a BUY order
TEST_F(OrderBookTest, AddBuyOrder)
{
    bool success = orderBook.addOrder(1, "Buy", 100, 10);
    ASSERT_TRUE(success);

    Order *order = orderBook.orders.find(1);
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->id, 1u);
    EXPECT_EQ(order->side, Side::BUY);
    EXPECT_EQ(order->price, 100);
    EXPECT_EQ(order->qty, 10);
}

// Test adding a SELL order
TEST_F(OrderBookTest, AddSellOrder)
{
    bool success = orderBook.addOrder(2, "Sell", 150, 5);
    ASSERT_TRUE(success);

    Order *order = orderBook.orders.find(2);
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->id, 2u);
    EXPECT_EQ(order->side, Side::SELL);
    EXPECT_EQ(order->price, 150);
    EXPECT_EQ(order->qty, 5);
}

// Test adding multiple orders at different prices
TEST_F(OrderBookTest, AddMultipleOrders)
{
    ASSERT_TRUE(orderBook.addOrder(1, "Buy", 100, 10));
    ASSERT_TRUE(orderBook.addOrder(2, "Buy", 105, 20));
    ASSERT_TRUE(orderBook.addOrder(3, "Sell", 110, 15));

    ASSERT_NE(orderBook.orders.find(1), nullptr);
    ASSERT_NE(orderBook.orders.find(2), nullptr);
    ASSERT_NE(orderBook.orders.find(3), nullptr);
}

// Test order cancellation (partial)
TEST_F(OrderBookTest, PartialCancelOrder)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    Order *order = orderBook.orders.find(1);
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->qty, 10);

    bool success = orderBook.orderCancel(1, 3);
    ASSERT_TRUE(success);
    EXPECT_EQ(order->qty, 7);
}

// Test order cancellation (full)
TEST_F(OrderBookTest, FullCancelOrder)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    ASSERT_NE(orderBook.orders.find(1), nullptr);

    bool success = orderBook.orderCancel(1, 10);
    ASSERT_TRUE(success);
    EXPECT_EQ(orderBook.orders.find(1), nullptr);
}

// Test partial order execution
TEST_F(OrderBookTest, PartialExecute)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    Order *order = orderBook.orders.find(1);
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->qty, 10);

    bool success = orderBook.orderExecuted(1, 3);
    ASSERT_TRUE(success);
    EXPECT_EQ(order->qty, 7);
}

// Test full order execution
TEST_F(OrderBookTest, FullExecute)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    ASSERT_NE(orderBook.orders.find(1), nullptr);

    bool success = orderBook.orderExecuted(1, 10);
    ASSERT_TRUE(success);
    EXPECT_EQ(orderBook.orders.find(1), nullptr);
}

// Test order delete
TEST_F(OrderBookTest, DeleteOrder)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    ASSERT_NE(orderBook.orders.find(1), nullptr);

    bool success = orderBook.orderDelete(1);
    ASSERT_TRUE(success);
    EXPECT_EQ(orderBook.orders.find(1), nullptr);
}

// Test cancel and replace
TEST_F(OrderBookTest, CancelReplace)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    ASSERT_NE(orderBook.orders.find(1), nullptr);

    bool success = orderBook.orderCancelReplace(1, 2, 105, 15);
    ASSERT_TRUE(success);

    EXPECT_EQ(orderBook.orders.find(1), nullptr);
    Order *newOrder = orderBook.orders.find(2);
    ASSERT_NE(newOrder, nullptr);
    EXPECT_EQ(newOrder->id, 2u);
    EXPECT_EQ(newOrder->price, 105);
    EXPECT_EQ(newOrder->qty, 15);
}

// Test best bid tracking
TEST_F(OrderBookTest, BestBidTracking)
{
    orderBook.addOrder(1, "Buy", 100, 10);
    EXPECT_EQ(orderBook.best_bid_price, 100);
    EXPECT_EQ(orderBook.best_bid_qty, 10);

    orderBook.addOrder(2, "Buy", 105, 20);
    EXPECT_EQ(orderBook.best_bid_price, 105);
    EXPECT_EQ(orderBook.best_bid_qty, 20);
}

// Test best ask tracking
TEST_F(OrderBookTest, BestAskTracking)
{
    orderBook.addOrder(1, "Sell", 150, 10);
    EXPECT_EQ(orderBook.best_ask_price, 150);
    EXPECT_EQ(orderBook.best_ask_qty, 10);

    orderBook.addOrder(2, "Sell", 145, 20);
    EXPECT_EQ(orderBook.best_ask_price, 145);
    EXPECT_EQ(orderBook.best_ask_qty, 20);
}

// Test price out of range
TEST_F(OrderBookTest, PriceOutOfRange)
{
    bool success = orderBook.addOrder(1, "Buy", 20000, 10);
    EXPECT_FALSE(success);
    EXPECT_EQ(orderBook.orders.find(1), nullptr);
}