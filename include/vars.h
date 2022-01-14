#pragma once
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <vector>
std::size_t n = 18;
struct TX_Buffer{
    std::vector<uint8_t> buf(n, 0x00);
    uint8_t len=0;
    int timeout = 0;
};
struct RX_Buffer{
    std::vector<uint8_t> buf(n, 0x00);
    // std::chrono::_V2::system_clock::time_point received_time;
};

