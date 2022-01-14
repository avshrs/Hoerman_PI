#pragma once
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <vector>
struct TX_Buffer{
    std::vector<uint8_t> buf;
    // std::vector<uint8_t> buf = std::vector<uint8_t>(18, 0x00);
    uint8_t len=0;
    int timeout = 0;
};
struct RX_Buffer{
    std::vector<uint8_t> buf;
    // std::chrono::_V2::system_clock::time_point received_time;
};

