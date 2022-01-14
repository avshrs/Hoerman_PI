
#include <unistd.h>
#include <chrono>
#include <vector>
struct TX_Buffer{
    std::vector<uint8_t> buf = {0};
    uint8_t len=0;
    int timeout = 0;
};
struct RX_Buffer{
    std::vector<uint8_t> buf = {0};
    // std::chrono::_V2::system_clock::time_point received_time;
};

