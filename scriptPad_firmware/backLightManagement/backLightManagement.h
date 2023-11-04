#ifndef BACKLIGHTMANAGEMENT
#define BACKLIGHTMANAGEMENT

#include <PicoLed.hpp>

#include <Effects/Marquee.hpp>
#include <Effects/Stars.hpp>
#include <Effects/Comet.hpp>
#include <Effects/Bounce.hpp>
#include <Effects/Particles.hpp>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

class backlightmanagement{
    public:
        backlightmanagement(uint8_t _pinNumber, uint8_t _ledNum) : \
                            pinNumber(_pinNumber), ledNum(_ledNum), ledMatrix(nullptr) {};
        void init();
        void setBackgroundEffect(uint8_t backgroundEffect);
        void backGroundLightTask();
        void setQueue(QueueHandle_t *_backLightPushQueue){backLightPushQueue = _backLightPushQueue;};

    private:
        uint8_t pinNumber;
        uint8_t ledNum;
        PicoLed::PicoLedController ledMatrix;

        QueueHandle_t *backLightPushQueue;

        std::vector<PicoLed::Color> rainbowPalette;
        std::vector<PicoLed::Color> firePalette;

        uint8_t effectSelected;

        uint effectIndex = 0;
        uint effectCount = 5;
        bool effectReset = false;
        uint32_t effectChangeInterval = 20000;  // Change effect every 20 seconds
        uint32_t timeNow = to_ms_since_boot( get_absolute_time() );
        uint32_t timeNext = timeNow + effectChangeInterval;       
};

#endif