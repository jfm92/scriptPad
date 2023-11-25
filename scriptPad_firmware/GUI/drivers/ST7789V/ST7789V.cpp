#include "ST7789V.h"

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include <map>
#include <vector>

bool ST7789V::init()
{
    spi_init(spi1, baudRate);
    gpio_set_function(sckGPIO, GPIO_FUNC_SPI);
    gpio_set_function(mosiGPIO, GPIO_FUNC_SPI);

    gpio_init(csGPIO);
    gpio_init(rstGPIO);
    gpio_init(dcGPIO);

    gpio_set_dir(csGPIO, GPIO_OUT);
    gpio_set_dir(rstGPIO, GPIO_OUT);
    gpio_set_dir(dcGPIO, GPIO_OUT);

    gpio_put(csGPIO, 1);
    gpio_put(dcGPIO, 0);

    reset();
    sendDisplayInitialization();

    return true;
}

void ST7789V::clear(uint16_t color)
{
    uint16_t image[resolutionHeight];

	memset(image, (color>>8 | (color&0xff)<<8), resolutionHeight);

    uint8_t *pixel = (uint8_t *)image;
    setWindows(0,0,resolutionHeight, resolutionWidth);

    gpio_put(dcGPIO, 1);
    gpio_put(csGPIO, 0);

    for(int i = 0; i< resolutionWidth; i++)
    {
        sendDataN(pixel, resolutionHeight*2);
    }
    gpio_put(csGPIO, 1);

}

void ST7789V::draw(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint8_t * color_p)
{
	setWindows(startX,startY,endX, endY);

    //Maybe we don't fill all the buffer, so we need to calculate how many pixels we're write into buffer;
    uint64_t dataSize = (endX - startX) * (endY -startY) * 2;

    gpio_put(dcGPIO, 1);
    gpio_put(csGPIO, 0);

    sendDataN(color_p, dataSize);

    gpio_put(csGPIO, 1);

}

void ST7789V::reset()
{
    gpio_put(rstGPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_put(rstGPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_put(rstGPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void ST7789V::setScreenConfig()
{
    uint8_t memoryAcceessAddr = 0x70; //0x00 for horizontal
    sendCommand(0x36);
    sendData(memoryAcceessAddr);
}

void ST7789V::sendCommand(uint8_t reg)
{
    gpio_put(dcGPIO, 0);
    gpio_put(csGPIO, 0);

    spi_write_blocking(SPI_PORT, &reg, 1);

    gpio_put(csGPIO, 1);
}

void ST7789V::sendData(uint8_t reg)
{
    gpio_put(dcGPIO, 1);
    gpio_put(csGPIO, 0);

    spi_write_blocking(SPI_PORT, &reg, 1);

    gpio_put(csGPIO, 1);
}

void ST7789V::sendDataN(uint8_t reg[], uint32_t dataLenght)
{
    spi_write_blocking(SPI_PORT, reg, dataLenght);
}

void ST7789V::sendDisplayInitialization()
{	
	//Command code , Data to send
	std::multimap<uint8_t, std::vector<uint8_t>> initList = {
        {0x36, {0x70}}, //Set screen orientation
        {0x3A, {0x05}},
        {0x21, {}},
        {0x2A, {0x00, 0x00, 0x01, 0x3F}},
        {0x2B, {0x00, 0x00, 0x00, 0xEF}},
        {0xB2, {0x0C, 0x0C, 0x00, 0x33, 0x33}},
        {0xB7, {0x35}},
        {0xBB, {0x1F}},
        {0xC0, {0x2C}},
        {0xC2, {0x01}},
        {0xC3, {0x12}},
        {0xC4, {0x20}},
        {0xC6, {0x0F}},
        {0xD0, {0xA4, 0xA1}},
        {0xE0, {0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D}},
        {0xE1, {0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31}},
        {0x21, {}},
        {0x11, {}},
        {0x29, {}}
    };

	for (auto entry : initList)
	{
		sendCommand(static_cast<uint8_t>(entry.first));
		if(!entry.second.empty())
		{
			for (auto data : entry.second)
			{
				sendData(static_cast<uint8_t>(data));
       		}
		}
    }
}

void ST7789V::setWindows(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY)
{
    sendCommand(0x2A);
    sendData(startX >>8);
    sendData(startX & 0xff);
	sendData((endX - 1) >> 8);
    sendData((endX-1) & 0xFF);

    //set the Y coordinates
    sendCommand(0x2B);
    sendData(startY >>8);
	sendData(startY & 0xff);
	sendData((endY - 1) >> 8);
    sendData((endY - 1) & 0xff);

    sendCommand(0X2C);
}

