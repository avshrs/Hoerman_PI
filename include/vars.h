
#include <unistd.h>
#include <chrono>

struct TX_Buffer{
    uint8_t buf[6]={0};
    uint8_t len=0;
    std::chrono::_V2::system_clock::time_point received_time;
};
struct RX_Buffer{
    uint8_t buf[6]={0};
    std::chrono::_V2::system_clock::time_point received_time;
};

