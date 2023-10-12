#include <map>


std::multimap<uint8_t, std::pair<uint8_t, uint8_t>> switchMatrix = {
    std::make_pair(15, std::make_pair(0x00, 13)),
    std::make_pair(15, std::make_pair(0x01, 12)),
    std::make_pair(14, std::make_pair(0x02, 13)),
};


std::map<uint8_t, uint8_t> gpioConfigEncoder = {
    {0, 16},
    {1, 17},
    {2, 18},
};