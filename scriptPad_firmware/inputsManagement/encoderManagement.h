#ifndef ENCODER_MANAGEMENT
#define ENCODER_MANAGEMENT

#include "switchManagement.h"

class encoderManagement : public  switchManagement
{
    public:
        encoderManagement(){}
        bool isRotating(uint8_t *rotationDirection);
        bool isPushed();
};

#endif