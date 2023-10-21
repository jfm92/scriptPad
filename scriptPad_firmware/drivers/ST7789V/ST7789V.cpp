#include "ST7789V.h"

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#define SPI_PORT spi1
#define LCD_RST_PIN  12
#define LCD_DC_PIN   8
    
#define LCD_CS_PIN   9
#define LCD_CLK_PIN  10
#define LCD_MOSI_PIN 11

bool ST7789V::init(/*uint16_t height, uint16_t width*/)
{
    spi_init(SPI_PORT, 10000 * 1000);
    gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);

    gpio_init(LCD_CS_PIN);
    gpio_init(LCD_RST_PIN);
    gpio_init(LCD_DC_PIN);
    gpio_set_dir(LCD_CS_PIN, GPIO_OUT);
    gpio_set_dir(LCD_RST_PIN, GPIO_OUT);
    gpio_set_dir(LCD_DC_PIN, GPIO_OUT);

    gpio_put(LCD_CS_PIN, 1);
    gpio_put(LCD_DC_PIN, 0);

    reset();
    sendDisplayInitialization();

    return true;
}

void ST7789V::clear(uint16_t color)
{
    uint16_t image[240];
    int i = 0;

    for(i = 0; i < 240; i++)
    {
        image[i] =color>>8 | (color&0xff)<<8;
    }

    uint8_t *pixel = (uint8_t *)image;
    setWindows(0,0,240, 320);

    gpio_put(LCD_DC_PIN, 1);
    gpio_put(LCD_CS_PIN, 0);

    for(i = 0; i< 320; i++)
    {
        sendDataN(pixel, 240*2);
    }
    gpio_put(LCD_CS_PIN, 1);

}

void ST7789V::reset()
{
    gpio_put(LCD_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_put(LCD_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_put(LCD_RST_PIN, 1);
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
    gpio_put(LCD_DC_PIN, 0);
    gpio_put(LCD_CS_PIN, 0);

    spi_write_blocking(SPI_PORT, &reg, 1);

    gpio_put(LCD_CS_PIN, 1);
}

void ST7789V::sendData(uint8_t reg)
{
    gpio_put(LCD_DC_PIN, 1);
    gpio_put(LCD_CS_PIN, 0);

    spi_write_blocking(SPI_PORT, &reg, 1);

    gpio_put(LCD_CS_PIN, 1);
}

void ST7789V::sendDataN(uint8_t reg[], uint32_t dataLenght)
{
    spi_write_blocking(SPI_PORT, reg, dataLenght);
}

void ST7789V::sendDisplayInitialization()
{
    sendCommand(0x36);
	sendData(0x00); 

	sendCommand(0x3A); 
	sendData(0x05);

	sendCommand(0x21); 

	sendCommand(0x2A);
	sendData(0x00);
	sendData(0x00);
	sendData(0x01);
	sendData(0x3F);

	sendCommand(0x2B);
	sendData(0x00);
	sendData(0x00);
	sendData(0x00);
	sendData(0xEF);

	sendCommand(0xB2);
	sendData(0x0C);
	sendData(0x0C);
	sendData(0x00);
	sendData(0x33);
	sendData(0x33);

	sendCommand(0xB7);
	sendData(0x35); 

	sendCommand(0xBB);
	sendData(0x1F);

	sendCommand(0xC0);
	sendData(0x2C);

	sendCommand(0xC2);
	sendData(0x01);

	sendCommand(0xC3);
	sendData(0x12);   

	sendCommand(0xC4);
	sendData(0x20);

	sendCommand(0xC6);
	sendData(0x0F); 

	sendCommand(0xD0);
	sendData(0xA4);
	sendData(0xA1);

	sendCommand(0xE0);
	sendData(0xD0);
	sendData(0x08);
	sendData(0x11);
	sendData(0x08);
	sendData(0x0C);
	sendData(0x15);
	sendData(0x39);
	sendData(0x33);
	sendData(0x50);
	sendData(0x36);
	sendData(0x13);
	sendData(0x14);
	sendData(0x29);
	sendData(0x2D);

	sendCommand(0xE1);
	sendData(0xD0);
	sendData(0x08);
	sendData(0x10);
	sendData(0x08);
	sendData(0x06);
	sendData(0x06);
	sendData(0x39);
	sendData(0x44);
	sendData(0x51);
	sendData(0x0B);
	sendData(0x16);
	sendData(0x14);
	sendData(0x2F);
	sendData(0x31);
	sendCommand(0x21);

	sendCommand(0x11);

	sendCommand(0x29);
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

