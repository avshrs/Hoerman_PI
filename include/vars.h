#pragma once
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <vector>

struct TX_Buffer{
    std::vector<uint8_t> buf;
    int timeout = 0;
};
struct RX_Buffer{
    std::vector<uint8_t> buf;
};

