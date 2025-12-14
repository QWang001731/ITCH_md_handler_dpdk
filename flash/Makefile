CXX = clang++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17
GTEST_PREFIX = /opt/homebrew/opt/googletest
GTEST_CXXFLAGS = -Iinclude -isystem $(GTEST_PREFIX)/include -Wall -Wextra -std=c++17
GTEST_LDFLAGS = -L$(GTEST_PREFIX)/lib -lgtest -lgtest_main -pthread

SRC = src/main.cc src/orderbook.cc
TEST_SRC = tests/test_orderbook.cc src/orderbook.cc
OBJ = $(SRC:.cc=.o)
TEST_OBJ = $(TEST_SRC:.cc=.o)
DEPS = $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)

TARGET = flash
TEST_TARGET = test_orderbook

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) -o $@ $^ $(GTEST_LDFLAGS)

# Rule for compiling test files with gtest includes
tests/%.o: tests/%.cc
	$(CXX) $(GTEST_CXXFLAGS) -MMD -MP -c $< -o $@

# Rule for compiling source files
src/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET) $(DEPS) src/*.d tests/*.d

.PHONY: all test clean