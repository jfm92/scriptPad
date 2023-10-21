#ifndef ST7789V_DRIVER
#define ST7789V_DRIVER

#include "stdint.h"

class ST7789V
{
    public:
        ST7789V(/*uint8_t _sckGPIO, uint8_t _mosiGPIO, uint8_t _csGPIO, uint8_t _rstGPIO, uint8_t _dcGPIO*/){}
        bool init(/*uint16_t height, uint16_t width*/);
        void clear(uint16_t color);


    private:
        uint8_t sckGPIO;
        uint8_t mosiGPIO;
        uint8_t csGPIO;
        uint8_t rstGPIO;
        uint8_t dcGPIO;
        uint16_t resolutionHeight;
        uint16_t resolutionWidth;

        void reset();
        void setScreenConfig();
        void sendCommand(uint8_t reg);
        void sendData(uint8_t reg);
        void sendDataN(uint8_t reg[], uint32_t dataLenght);
        void sendDisplayInitialization();
        void setWindows(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY);

};



#endif