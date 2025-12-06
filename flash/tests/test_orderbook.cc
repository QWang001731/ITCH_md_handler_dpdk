#include <gtest/gtest.h>
#include "orderbook.h"

class OrderBookTest : public ::testing::Test {
protected:
    OrderBook orderBook;

    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(OrderBookTest, AddOrder) {
    // Test adding an order
    orderBook.addOrder("AAPL", 10, 150.0);
    auto orders = orderBook.getOrders();
    ASSERT_EQ(orders.size(), 1);
    EXPECT_EQ(orders[0].symbol, "AAPL");
    EXPECT_EQ(orders[0].quantity, 10);
    EXPECT_EQ(orders[0].price, 150.0);
}

TEST_F(OrderBookTest, RemoveOrder) {
    // Test removing an order
    orderBook.addOrder("AAPL", 10, 150.0);
    orderBook.removeOrder("AAPL", 10);
    auto orders = orderBook.getOrders();
    ASSERT_EQ(orders.size(), 0);
}

TEST_F(OrderBookTest, GetOrders) {
    // Test getting orders
    orderBook.addOrder("AAPL", 10, 150.0);
    orderBook.addOrder("GOOGL", 5, 2800.0);
    auto orders = orderBook.getOrders();
    ASSERT_EQ(orders.size(), 2);
}