#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdint.h"
#include "stdint.h"
#include "pico/stdlib.h"

#include "hwConfig.h"

#include "inputsManagement/switchManagement.h"
#include "inputsManagement/encoderManagement.h"
#include "HIDManagement/HIDManagement.h"
#include "GUI/GUI.h"
#include "GUI/drivers/ST7789V/ST7789V.h"
#include "filesManagement/filesManagement.h"
#include "modemUSBManagement/modemUSBManagement.h"
#include "backLightManagement/backLightManagement.h"

#include <string>
#include <list>
#include <bits/stdc++.h>
#include "libraries/cJSON/cJSON.h"
#include "rtc.h"

modemUSBManagement* modemUSBManagement::instanceModemUSB = NULL; 
filesManagement* filesManagement::instanceFileManagement = NULL;
switchManagement* switchManagement::instance = NULL;
encoderManagement* encoderManagement::instance = NULL;
HIDManagement* HIDManagement::instanceHID = NULL; 
ST7789V* ST7789V::instaceST7789V = NULL;
GUI* GUI::instanceGUI = NULL; 

QueueHandle_t HIDPushQueue;
QueueHandle_t GUIPushQueue;
QueueHandle_t backLightPushQueue;

TaskHandle_t managerTask_Handle;
TaskHandle_t displayTask_Handle;
TaskHandle_t modemUSBTask_Handle;
TaskHandle_t HIDTask_Handle;
TaskHandle_t backgroundLigth_Handle;

void USBModemTaskFun(__unused void *params)
{
    modemUSBManagement& instanceModemUSB = modemUSBManagement::getInstance();
    instanceModemUSB.initConfigManager();

    while(1)
    {
        instanceModemUSB.taskConfig();
    }
}

void HIDTaskFun(__unused void *params)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    HIDManagement& managementHID = HIDManagement::getInstance();
    managementHID.initHID();
    managementHID.setMessageQueue(&HIDPushQueue);
    
    while(true) {
       managementHID.taskHID();
    }
}

void managerTaskFun(__unused void *params)
{
    std::string previousMacroUsed;
    
    //Display initialization
    ST7789V& displayInstance = ST7789V::getInstance();
    displayInstance.init();
    displayInstance.clear(0x0000);

    //Look for a previous configuration
    filesManagement& FSmanagementInstance = filesManagement::getInstance();
    FSmanagementInstance.initFileManagement();
    FSmanagementInstance.checkPreviousMacroUsed(&previousMacroUsed);

    if(!previousMacroUsed.empty())
    {
        //Fresh initialization
    }

    GUI& GUIInstance = GUI::getInstance();
    GUIInstance.init();
    GUIInstance.setQueue(&GUIPushQueue);
    GUIInstance.setHIDTask(&HIDTask_Handle);
    GUIInstance.setModemUSBTask(&modemUSBTask_Handle);


    while(1)
    {
        GUIInstance.LVGLTask();
    }
}


void backGroundLightTask(__unused void *params)
{
    backlightmanagement backligth(23,12);
    backligth.init();
    backligth.setBackgroundEffect(0x00);
    backligth.setQueue(&backLightPushQueue);

    while(1)
    {
        backligth.backGroundLightTask();
    }
}

int main(void)
{
    stdio_init_all();
    board_init();
    time_init();

    // Comment to use UART
    set_sys_clock_khz(250000, true);

    UBaseType_t uxCoreAffinityMask;

    //Internal queue initialization
    HIDPushQueue = xQueueCreate(5, sizeof(int8_t));
    GUIPushQueue = xQueueCreate(5, sizeof(int8_t));
    backLightPushQueue = xQueueCreate(5, sizeof(int8_t));

    //Peripherals and FS initialization
    switchManagement& switchManagementInstance = switchManagement::getInstance();
    
    switchManagementInstance.setHIDMessageQueue(&HIDPushQueue);
    switchManagementInstance.setGUIMessageQueue(&GUIPushQueue);
    switchManagementInstance.setSwitchConfig(&switchMatrix);
    switchManagementInstance.initGPIO();

    encoderManagement& encoderManagementInstance = encoderManagement::getInstance();
    
    encoderManagementInstance.setHIDMessageQueue(&HIDPushQueue);
    encoderManagementInstance.setGUIMessageQueue(&GUIPushQueue);
    encoderManagementInstance.setEncoderConfig(&gpioConfigEncoder);
    encoderManagementInstance.init();

    //Tasks Initialization
    //It must be this exact order
    xTaskCreate(managerTaskFun, "managerTask", 4096, NULL, configMAX_PRIORITIES-1, &managerTask_Handle);
    uxCoreAffinityMask = ( ( 1 << 0 ));
    vTaskCoreAffinitySet( managerTask_Handle, uxCoreAffinityMask );

    xTaskCreate(backGroundLightTask, "backGroundTask", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-5 , &backgroundLigth_Handle);
    uxCoreAffinityMask = ( ( 1 << 1 ));
    vTaskCoreAffinitySet( backgroundLigth_Handle, uxCoreAffinityMask );

    xTaskCreate(HIDTaskFun, "HIDTask", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1 , &HIDTask_Handle);
    vTaskSuspend(HIDTask_Handle); //We don't know if we're on config mode, so this must be suspend by now.

    xTaskCreate(USBModemTaskFun, "USBModem", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1 , &modemUSBTask_Handle);
    vTaskSuspend(modemUSBTask_Handle); //USB Modem task will never be initialize at the beginning

    vTaskStartScheduler();
    return 0;
}