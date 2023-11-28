#include <map>


std::multimap<uint8_t, std::pair<uint8_t, uint8_t>> switchMatrix = {
    std::make_pair(18, std::make_pair(0x00, 19)),
    std::make_pair(18, std::make_pair(0x01, 20)),
    std::make_pair(18, std::make_pair(0x02, 21)),
    std::make_pair(17, std::make_pair(0x03, 19)),
    std::make_pair(17, std::make_pair(0x04, 20)),
    std::make_pair(17, std::make_pair(0x05, 21)),
    std::make_pair(16, std::make_pair(0x06, 19)),
    std::make_pair(16, std::make_pair(0x07, 20)),
    std::make_pair(16, std::make_pair(0x08, 21)),
};


std::map<uint8_t, uint8_t> gpioConfigEncoder = {
    {0, 26}, //Encoder button
    {1, 27}, //Encoder contact A
    {2, 28}, //Encoder contact B
};