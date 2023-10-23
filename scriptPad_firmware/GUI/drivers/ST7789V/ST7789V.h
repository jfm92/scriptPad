#ifndef ST7789V_DRIVER
#define ST7789V_DRIVER

#include <utility>
#include "stdint.h"
#include "lvgl.h"
#include "hardware/spi.h"

class ST7789V
{
    public:
        ST7789V(uint8_t _sckGPIO, uint8_t _mosiGPIO, uint8_t _csGPIO, uint8_t _rstGPIO, uint8_t _dcGPIO)
        : sckGPIO(_sckGPIO), mosiGPIO(_mosiGPIO), csGPIO(_csGPIO), rstGPIO(_rstGPIO), dcGPIO(_dcGPIO){};

        void setBaudRate(uint32_t _baudRate) {
            baudRate = _baudRate;
        }

        void setResolution(uint16_t _resolutionHeight, uint16_t _resolutionWidth) {
            resolutionHeight = _resolutionHeight;
            resolutionWidth = _resolutionWidth;
        }

        void setBufferLines(uint8_t _bufferLines){
            bufferLines = _bufferLines;
        }

        uint32_t getBaudRate() {
            return baudRate;
        }

        std::pair<uint16_t, uint16_t> setResolution() {
            return {resolutionHeight, resolutionWidth};
        }

        uint8_t getBufferLines(){
            return bufferLines;
        }

        bool init(/*uint16_t height, uint16_t width*/);
        void clear(uint16_t color);
        void draw(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t * color_p);

    private:
        uint8_t sckGPIO;
        uint8_t mosiGPIO;
        uint8_t csGPIO;
        uint8_t rstGPIO;
        uint8_t dcGPIO;

        uint16_t resolutionHeight = 240;
        uint16_t resolutionWidth = 320;

        uint64_t baudRate = 80000 * 1000; //80MHZ

        uint8_t bufferLines = 10; // Num of lines to be saved

        spi_inst_t *SPI_PORT = spi1;


        void reset();
        void setScreenConfig();
        void sendCommand(uint8_t reg);
        void sendData(uint8_t reg);
        void sendDataN(uint8_t reg[], uint32_t dataLenght);
        void sendDisplayInitialization();
        void setWindows(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY);

};



#endif