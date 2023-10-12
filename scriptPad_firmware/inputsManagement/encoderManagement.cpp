#include "encoderManagement.h"
#include "pico/stdlib.h"

bool encoderManagement::isRotating(uint8_t *rotationDirection)
{
    //TODO: Maybe this should be check on a thread
    return false;
}

bool encoderManagement::isPushed()
{
    return !gpio_get(gpioConfig[2]);
}