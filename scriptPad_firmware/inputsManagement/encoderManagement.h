#ifndef ENCODER_MANAGEMENT
#define ENCODER_MANAGEMENT

#include "encoderManagement.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include <map>

class encoderManagement
{
    public:
        //Initialize encoder pins and controller thread.
        bool init();

        // Method to add intraprocess message queue with pushed switches
        void setHIDMessageQueue(QueueHandle_t *_encoderQUEUEInternal){ encoderQUEUEInternal = _encoderQUEUEInternal;};

        // Method to get intraprocess message queue with pushed switches
        QueueHandle_t *getHIDMessageQueue() {return encoderQUEUEInternal;};

        // Method to add intraprocess message queue with pushed switches
        void setGUIMessageQueue(QueueHandle_t *_GUIPushQueue){ GUIQUEUEInternal = _GUIPushQueue;};

        // Method to get intraprocess message queue with pushed switches
        QueueHandle_t *getGUIMessageQueue() {return GUIQUEUEInternal;};

        // Method to set GPIO matrix config
        void setEncoderConfig(std::map<uint8_t, uint8_t> *_gpioEncoder) {gpioEncoder = _gpioEncoder;};

        // Method to get GPIO matrix config, returns a std::multimap<uint8_t, std::pair<uint8_t, uint8_t>>
        auto *getEncoderConfig() {return gpioEncoder;};

        //Singletone class initialization
        static encoderManagement& getInstance()
        {
            if (instance == nullptr)
            {
                instance = new encoderManagement();
            }
            return *instance;
        }

        //Delete copy class operators
        encoderManagement(const encoderManagement&) = delete;
        encoderManagement& operator=(const encoderManagement&) = delete;
    
    private:
        static encoderManagement* instance;

        QueueHandle_t *encoderQUEUEInternal = nullptr;
        QueueHandle_t *GUIQUEUEInternal = nullptr;

        TaskHandle_t encoderTask_Handle;

        std::map<uint8_t, uint8_t> *gpioEncoder = nullptr;

        bool encoderInitialized = false;
        uint8_t longPushCyclesCount = 0;

        encoderManagement(){};

        bool getEnconderSwitchState() {
            auto gpioNum = gpioEncoder->find(0);
            return !gpio_get(gpioNum->second);
        };
        bool getEnconderContactAState() {
            auto gpioNum = gpioEncoder->find(1);
            return !gpio_get(gpioNum->second);
        };
        bool getEnconderContactBState() {
            auto gpioNum = gpioEncoder->find(2);
            return !gpio_get(gpioNum->second);
        };

        void increasePushCycle(){longPushCyclesCount ++;};
        void resetPushCycle(){longPushCyclesCount = 0;};
        bool isLongPush(){return (longPushCyclesCount > 5);};

        uint8_t getFunctionGPIO(uint8_t gpioCode){
            auto gpioNum = gpioEncoder->find(gpioCode);
            return gpioNum->second;
        }

        friend void encoderTask(void *param);
        friend int64_t longPressTimerCB(alarm_id_t id, void *user_data);
        friend void encoderButtonCB(uint gpio, uint32_t events);
};

#endif