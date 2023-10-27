#ifndef _GUI
#define _GUI

#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

class GUI
{
public:
    
    void setHIDQueue();
    void init();
    void LVGLTask();
    void setQueue(QueueHandle_t *_GUIPushQueue){GUIPushQueue = _GUIPushQueue;};
    QueueHandle_t *getQueue(){return GUIPushQueue;}
    void GUIControl(bool _available){ controlActive = _available;};
    bool GUIControlAvailable(){ return controlActive;};

    static GUI& getInstance()
        {
            if (instanceGUI == nullptr)
            {
                instanceGUI = new GUI();
            }
            return *instanceGUI;
        }

    //Delete copy class operators
    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

private:
    GUI(){};

    static GUI* instanceGUI;

    QueueHandle_t *GUIPushQueue;

    bool controlActive = true;

    lv_disp_draw_buf_t drawBuf1;
    lv_color_t buffer[320 * 10];
    lv_disp_drv_t dispDriver;
    lv_indev_drv_t inputDriver;
    lv_indev_t * inputDriverKeypad; 

};



#endif