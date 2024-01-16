#include <map>

// Emitting pin, switch ID, recv Pin
std::multimap<uint8_t, std::pair<uint8_t, uint8_t>> switchMatrix = {
    std::make_pair(19, std::make_pair(1, 20)),
    std::make_pair(18, std::make_pair(2, 20)),
    std::make_pair(17, std::make_pair(3, 20)),
    std::make_pair(16, std::make_pair(4, 20)),
    std::make_pair(19, std::make_pair(5, 21)),
    std::make_pair(18, std::make_pair(6, 21)),
    std::make_pair(17, std::make_pair(7, 21)),
    std::make_pair(16, std::make_pair(8, 21)),
    std::make_pair(19, std::make_pair(9, 22)),
    std::make_pair(18, std::make_pair(10, 22)),
    std::make_pair(17, std::make_pair(11, 22)),
    std::make_pair(16, std::make_pair(12, 22)),
};

// Switch ID, encoder Pin
std::map<uint8_t, uint8_t> gpioConfigEncoder = {
    {13, 26}, //Encoder switch -> ID 13 Push button
    {14, 27}, //Encoder contact A -> ID 14 Clockwise movement
    {15, 28}, //Encoder contact B -> ID 15 AntiClockwise movement
};