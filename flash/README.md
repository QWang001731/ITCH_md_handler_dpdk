# Orderbook Project

This project implements a simple trading order book system using C++. It includes a main application that utilizes the `OrderBook` and `SPSCQueue` classes for managing orders in a trading environment.

## Project Structure

```
orderbook-project
├── src
│   ├── main.cc          # Entry point of the application
│   ├── orderbook.cc     # Implementation of the OrderBook class
│   └── spscqueue.cc     # Implementation of the SPSCQueue class
├── include
│   ├── orderbook.h      # Header file for the OrderBook class
│   └── spscqueue.h      # Header file for the SPSCQueue class
├── tests
│   └── test_orderbook.cc # Unit tests for the OrderBook class
├── Makefile             # Build instructions for the project
├── .gitignore           # Files and directories to ignore by Git
└── README.md            # Documentation for the project
```

## Setup Instructions

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd orderbook-project
   ```

2. **Build the project**:
   Use the provided Makefile to compile the project:
   ```bash
   make
   ```

3. **Run the application**:
   After building, you can run the application with:
   ```bash
   ./orderbook_project
   ```

## Usage

The application manages a trading order book, allowing users to add, remove, and query orders. The `OrderBook` class provides methods for these operations, while the `SPSCQueue` class is used for handling order processing in a thread-safe manner.

## Testing

Unit tests for the `OrderBook` class are provided in the `tests/test_orderbook.cc` file. To run the tests, you can use a testing framework of your choice, or compile and run the test file separately.

## Contributing

Contributions are welcome! Please submit a pull request or open an issue for any enhancements or bug fixes.