#ifndef SWITCH_MANAGEMENT
#define SWITCH_MANAGEMENT

#include "queue.h"

class switchManagement
{
    public:
        switchManagement(QueueHandle_t *_switchsPushQueue);
        void initGPIO();
};

#endif