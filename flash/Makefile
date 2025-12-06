CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++11
SRC = src/main.cc src/orderbook.cc src/spscqueue.cc
OBJ = $(SRC:.cc=.o)
TARGET = flash

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean