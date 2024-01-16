#ifndef ENCODER_MANAGEMENT
#define ENCODER_MANAGEMENT

#include "encoderManagement.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include <map>
#include <vector>

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

        std::vector<uint8_t> encoderGPIOList;
        std::vector<uint8_t> encoderGPIOSWID;

        bool encoderInitialized = false;
        uint8_t longPushCyclesCount = 0;

        const int timeThreshold = 5;
        long timeCounter = 0;
    
        int32_t ISRCounterA = 0;
        int32_t ISRCounterB = 0;
        //Const with minimal pulsed to register an action
        const uint8_t minIntCount = 6; 


        encoderManagement(){};

        bool getEnconderSwitchState() { return !gpio_get(encoderGPIOList[0]);};
        bool getEnconderContactAState() {return !gpio_get(encoderGPIOList[1]);};
        bool getEnconderContactBState() {return !gpio_get(encoderGPIOList[2]);};

        void increasePushCycle(){longPushCyclesCount ++;};
        void resetPushCycle(){longPushCyclesCount = 0;};
        bool isLongPush(){return (longPushCyclesCount > 5);};

        friend int64_t longPressTimerCB(alarm_id_t id, void *user_data);
        friend void encoderCB(void);
};

#endif