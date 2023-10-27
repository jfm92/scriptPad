#ifndef SWITCH_MANAGEMENT
#define SWITCH_MANAGEMENT

//freeRTOS libraries
#include "queue.h"
#include <vector>
#include <map>
#include <set>
#include "pico/time.h"

class switchManagement
{
    public:
        
        // Initialize all GPIO with required configuration.
        void initGPIO();

        // Method to add intraprocess message queue with pushed switches
        void setHIDMessageQueue(QueueHandle_t *_switchsPushQueue){ switchQUEUEInternal = _switchsPushQueue;};

        // Method to get intraprocess message queue with pushed switches
        QueueHandle_t *getHIDMessageQueue() {return switchQUEUEInternal;};

        // Method to add intraprocess message queue with pushed switches
        void setGUIMessageQueue(QueueHandle_t *_GUIPushQueue){ GUIQUEUEInternal = _GUIPushQueue;};

        // Method to get intraprocess message queue with pushed switches
        QueueHandle_t *getGUIMessageQueue() {return GUIQUEUEInternal;};

        // Method to set GPIO matrix config
        void setSwitchConfig(std::multimap<uint8_t, std::pair<uint8_t, uint8_t>> *_switchMatrix) {switchMatrix = _switchMatrix;};

        // Method to get GPIO matrix config, returns a std::multimap<uint8_t, std::pair<uint8_t, uint8_t>>
        auto *getSwitchConfig() {return switchMatrix;};

        void setPollingTimeNs(uint64_t _pollingTime) {pollingTime = _pollingTime/1000;}

        float getPollingTimeNs() {return pollingTime;}
        
        //Singletone class initialization
        static switchManagement& getInstance()
        {
            if (instance == nullptr)
            {
                instance = new switchManagement();
            }
            return *instance;
        }

        //Delete copy class operators
        switchManagement(const switchManagement&) = delete;
        switchManagement& operator=(const switchManagement&) = delete;

    private:
        static switchManagement* instance;
        QueueHandle_t *switchQUEUEInternal = nullptr;
        QueueHandle_t *GUIQUEUEInternal = nullptr;
        std::vector<int8_t> gpioQueue;
        std::multimap<uint8_t, std::pair<uint8_t, uint8_t>> *switchMatrix = nullptr;
        bool gpiosInitialized = false;
        uint64_t pollingTime = 1;

        std::set<uint8_t> verticalGPIOUsed;
        std::set<uint8_t> horizontalGPIOUsed;

        switchManagement(){};
        void addGPIOPushed(int8_t pushedGPIO);
        int8_t getLastGPIOPushed();
        uint8_t gpioPushed;

        // We need to access private method from these CB global functions.
        friend void gpioCB(uint gpio, uint32_t events);
        friend int64_t alarmCB(alarm_id_t id, void *user_data); 
        friend void gpioTask(void *param);  
};

#endif