//#include <stdio.h>
#include <stdint.h>

#include <algorithm>

#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"

#include "switchManagement.h"
//#include "hwConfig.h"

////////////////////// Externals callbacks //////////////////////
int64_t alarmCB(alarm_id_t id, void *user_data);

void gpioCB(uint gpio, uint32_t events)
{
    switchManagement& switchManagementInstance = switchManagement::getInstance();
    QueueHandle_t *switchQUEUEInternal = switchManagementInstance.getMessageQueue();

    BaseType_t xHigherPrioritTaskWoken = pdFALSE;
    //De-initialize to avoid switch bounching
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , false, &gpioCB);

    auto range = switchManagementInstance.getSwitchConfig()->equal_range(gpio);

    // We check which horizontal pins are pushed and if so, send switch id over the message queue
    for (auto iter = range.first; iter != range.second; ++iter) {
        if(!gpio_get(iter->second.second))
        {
            uint8_t ID = iter->second.first;
            if(switchQUEUEInternal != nullptr)
            {
                xQueueSendToFrontFromISR(*switchQUEUEInternal, &ID, &xHigherPrioritTaskWoken );
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

////////////////////// Externals callbacks //////////////////////

void switchManagement::initGPIO() {

    if((switchMatrix== nullptr) || gpiosInitialized ) return;

    std::set<uint8_t> verticalGPIOUsed;
    std::set<uint8_t> horizontalGPIOUsed;

    for (const auto& entry : *switchMatrix) {
        uint8_t verticalPin = entry.first;
        uint8_t horizontalPin = entry.second.second;

        if (verticalGPIOUsed.find(verticalPin) == verticalGPIOUsed.end()) {
            // Set pull-up and callback ISR config for the vertical pin.
            gpio_set_pulls(verticalPin, true, false);
            gpio_set_irq_enabled_with_callback(verticalPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpioCB);

            verticalGPIOUsed.insert(verticalPin);
        }

        if (horizontalGPIOUsed.find(horizontalPin) == horizontalGPIOUsed.end()) {
            // Set pull-up for the horizontal pin.
            gpio_set_pulls(horizontalPin, true, false);

            horizontalGPIOUsed.insert(horizontalPin);
        }
    }
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