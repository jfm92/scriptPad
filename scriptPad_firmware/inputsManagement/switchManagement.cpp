#include <stdio.h>
#include <stdint.h>
#include <map>
#include <set>
#include <algorithm>
#include <vector>

#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"

#include "switchManagement.h"
#include "hwConfig.h"

QueueHandle_t *switchQUEUEInternal;
std::vector<uint8_t> gpioQueue;

int64_t alarm_callback(alarm_id_t id, void *user_data);

void pin_callback(uint gpio, uint32_t events) {
    BaseType_t xHigherPrioritTaskWoken = pdFALSE;
    //De-initialize to avoid switch bounching
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , false, &pin_callback);

    auto range = switchMatrix.equal_range(gpio);

    // We check which horizontal pins are pushed and if so, send switch id over the message queue
    for (auto iter = range.first; iter != range.second; ++iter) {
        if(!gpio_get(iter->second.second))
        {
            uint8_t ID = iter->second.first;
            xQueueSendToFrontFromISR(*switchQUEUEInternal, &ID, &xHigherPrioritTaskWoken );
        }
    }

    // We need to save on a global vector which gpio was de-initialize due to we will lose on alarm callback 
    // because this ISR function will be finished.
    gpioQueue.push_back(gpio);
    add_alarm_in_ms(250, alarm_callback, NULL, false); //Wait 250 ms until we active again that pin.
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    uint8_t gpioReactive = gpioQueue.back();
    gpioQueue.pop_back();
    gpio_set_irq_enabled_with_callback(gpioReactive, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL , true, &pin_callback);

    return 0;
}

switchManagement::switchManagement(QueueHandle_t *_switchsPushQueue)
{
    switchQUEUEInternal = _switchsPushQueue;
}

void switchManagement::initGPIO() {
    std::set<uint8_t> verticalGPIOUsed;
    std::set<uint8_t> horizontalGPIOUsed;

    for (const auto& entry : switchMatrix) {
        uint8_t verticalPin = entry.first;
        uint8_t horizontalPin = entry.second.second;

        if (verticalGPIOUsed.find(verticalPin) == verticalGPIOUsed.end()) {
            // Set pull-up and EDGE callback ISR config for the vertical pin.
            gpio_set_pulls(verticalPin, true, false);
            gpio_set_irq_enabled_with_callback(verticalPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &pin_callback);

            verticalGPIOUsed.insert(verticalPin);
        }

        if (horizontalGPIOUsed.find(horizontalPin) == horizontalGPIOUsed.end()) {
            // Set pull-up for the horizontal pin.
            gpio_set_pulls(horizontalPin, true, false);

            horizontalGPIOUsed.insert(horizontalPin);
        }
    }
}