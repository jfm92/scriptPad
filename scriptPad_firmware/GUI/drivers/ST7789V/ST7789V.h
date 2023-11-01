#ifndef ST7789V_DRIVER
#define ST7789V_DRIVER

#include <utility>
#include "stdint.h"
#include "lvgl.h"
#include "hardware/spi.h"

class ST7789V
{
    public:
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
        void draw(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint8_t * color_p);

        //Singletone class initialization
        static ST7789V& getInstance()
        {
            if (instaceST7789V == nullptr)
            {
                instaceST7789V = new ST7789V();
            }
            return *instaceST7789V;
        }

        //Delete copy class operators
        ST7789V(const ST7789V&) = delete;
        ST7789V& operator=(const ST7789V&) = delete;

    private:
        uint8_t sckGPIO = 10;
        uint8_t mosiGPIO = 11;
        uint8_t csGPIO = 9;
        uint8_t rstGPIO = 12;
        uint8_t dcGPIO = 8;

        uint16_t resolutionHeight = 240;
        uint16_t resolutionWidth = 320;

        uint64_t baudRate = 100000 * 1000; //80MHZ

        uint8_t bufferLines = 20; // Num of lines to be saved

        spi_inst_t *SPI_PORT = spi1;

        static ST7789V* instaceST7789V;

        ST7789V(){};

        void reset();
        void setScreenConfig();
        void sendCommand(uint8_t reg);
        void sendData(uint8_t reg);
        void sendDataN(uint8_t reg[], uint32_t dataLenght);
        void sendDisplayInitialization();
        void setWindows(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY);

};



#endif