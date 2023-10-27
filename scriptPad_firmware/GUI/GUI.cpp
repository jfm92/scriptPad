#include "GUI.h"

#include "drivers/ST7789V/ST7789V.h"

#include "FreeRTOS.h"
#include "queue.h"
#include <timers.h>

#include "stdio.h"

static lv_group_t * group_interact;
static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;

/**********************Static functions required by LVGL*****************************************/
static void displayFlush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    ST7789V& display = ST7789V::getInstance();

    display.draw(area->x1,
            area->y1,               
            area->x2 - area->x1 + 1,   
            area->y2 - area->y1 + 1, 
            (uint16_t *)color_p);
    
    lv_disp_flush_ready(disp_drv);
}

static void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{ 
    uint8_t recvSwitchCode;
    GUI& GUIInstance = GUI::getInstance();
    bool controlAvailable = GUIInstance.GUIControlAvailable();

    if (controlAvailable && xQueueReceive(*GUIInstance.getQueue(), &recvSwitchCode, 0)) {
        
    }
}

void lvglTick(void* param)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_tick_inc(10);
    }
}


void GUI::init()
{
    lv_init();
    lv_disp_draw_buf_init(&drawBuf1, buffer, NULL, 320 * 10);
    lv_disp_drv_init(&dispDriver);

    dispDriver.hor_res = 320;
    dispDriver.ver_res = 240;

    dispDriver.flush_cb = displayFlush;
    dispDriver.draw_buf = &drawBuf1;  
    lv_disp_drv_register(&dispDriver);

    lv_indev_drv_init(&inputDriver);
    inputDriver.type = LV_INDEV_TYPE_KEYPAD;
    inputDriver.read_cb = my_input_read;
    inputDriverKeypad = lv_indev_drv_register(&inputDriver);



    xTaskCreate(lvglTick, "TCI", 256, NULL,configMAX_PRIORITIES-1 , NULL);    
}

void GUI::LVGLTask()
{
    lv_timer_handler();
}