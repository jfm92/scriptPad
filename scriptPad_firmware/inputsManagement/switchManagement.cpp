#include <stdio.h>
#include <stdint.h>

#include <algorithm>

#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "switchManagement.h"

////////////////////// Externals callbacks //////////////////////
int64_t alarmCB(alarm_id_t id, void *user_data);

void gpioCB(uint gpio, uint32_t events)
{
    switchManagement& switchManagementInstance = switchManagement::getInstance();
    QueueHandle_t *switchQUEUEInternal = switchManagementInstance.getHIDMessageQueue();
    QueueHandle_t *GUIQUEUEInternal = switchManagementInstance.getGUIMessageQueue();

    BaseType_t xHigherPrioritTaskWoken = pdFALSE;
    //De-initialize to avoid switch bounching
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , false, &gpioCB);

    auto range = switchManagementInstance.getSwitchConfig()->equal_range(gpio);

    // We check which horizontal pins are pushed and if so, send switch id over the message queue
    for (auto iter = range.first; iter != range.second; ++iter) {
        if(iter->second.second== switchManagementInstance.gpioPushed)
        {
            uint8_t ID = iter->second.first;
            if(switchQUEUEInternal != nullptr)
            {
                xQueueSendToFrontFromISR(*switchQUEUEInternal, &ID, &xHigherPrioritTaskWoken );
                xQueueSendToFrontFromISR(*GUIQUEUEInternal, &ID, &xHigherPrioritTaskWoken );
            }
        }
    }

    // We need to save on a global vector which gpio was de-initialize due to we will lose on alarm callback 
    // because this ISR function will be finished.
    switchManagementInstance.addGPIOPushed(gpio);
    add_alarm_in_ms(250, alarmCB, NULL, false); //Wait 250 ms until we active again that pin.
}

int64_t alarmCB(alarm_id_t id, void *user_data) 
{
    switchManagement& switchManagementInstance = switchManagement::getInstance();
    int8_t gpioToReactive= switchManagementInstance.getLastGPIOPushed();

    // If value is -1, means that we don't have any value in the queue. This is an unexpected situation, but just in case.
    if(gpioToReactive > -1)
    {
        gpio_set_irq_enabled_with_callback(gpioToReactive, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &gpioCB);
    }
    
    return 0;
}

void gpioTask(void *param)
{
    switchManagement& switchManagementInstance = switchManagement::getInstance();

    while(1)
    {   
        float pollingTime = switchManagementInstance.getPollingTimeNs();
        for(uint8_t gpioPin : switchManagementInstance.horizontalGPIOUsed)
        {
            switchManagementInstance.gpioPushed = gpioPin;
            gpio_put(gpioPin, 1);
            vTaskDelay(pdMS_TO_TICKS(pollingTime));
            gpio_put(gpioPin, 0);
            vTaskDelay(pdMS_TO_TICKS(pollingTime));
        }
        
    }
}

////////////////////// Externals callbacks End //////////////////////

void switchManagement::initGPIO() {

    if((switchMatrix== nullptr) || gpiosInitialized ) return;

    for (const auto& entry : *switchMatrix) {
        uint8_t verticalPin = entry.first;
        uint8_t horizontalPin = entry.second.second;

        if (verticalGPIOUsed.find(verticalPin) == verticalGPIOUsed.end()) {
            // Set pull-up and callback ISR config for the vertical pin.
            gpio_set_pulls(verticalPin, false, true);
            gpio_set_irq_enabled_with_callback(verticalPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpioCB);

            verticalGPIOUsed.insert(verticalPin);
        }

        if (horizontalGPIOUsed.find(horizontalPin) == horizontalGPIOUsed.end()) {
            // Set pull-up for the horizontal pin.
            gpio_init(horizontalPin);
            gpio_set_dir(horizontalPin, true);

            horizontalGPIOUsed.insert(horizontalPin); //Might be some values are duplicated, so we don't want to initialize twice
        }
    }

    xTaskCreate( gpioTask, "gpioControl", 4096, NULL, configMAX_PRIORITIES-1, NULL);
}

void switchManagement::addGPIOPushed(int8_t pushedGPIO)
{
    gpioQueue.push_back(pushedGPIO);
}

int8_t switchManagement::getLastGPIOPushed()
{
    int8_t gpioBlocked = -1;

    if(!gpioQueue.empty())
    {
        gpioBlocked = gpioQueue.back();
        gpioQueue.pop_back();
    }

    return gpioBlocked;
}