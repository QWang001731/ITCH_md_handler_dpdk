#pragma once
#include "ItchMsg.h"
#include "InstrumentManager.h"
#include <cstdint>

class InstrumentManager
{
public:
    OrderBook &book(InstrumentId id);
    InstrumentId id_from_symbol(std::string_view sym);

private:
    std::vector<OrderBook> books_;
    // maybe unordered_map<Symbol, InstrumentId> for mapping
};

class MarketDataFeedHandler
{
public:
    void on_bytes(const uint8_t *data, size_t len);

private:
    void on_message(const ItchMsg &m); // dispatch
    InstrumentManager *instruments_;   // map stock -> instrument_id -> OrderBook*
};
