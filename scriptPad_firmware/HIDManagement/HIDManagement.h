#ifndef HID_MANAGEMENT
#define HID_MANAGEMENT

#include "FreeRTOS.h"
#include "queue.h"
#include <timers.h>

#include "bsp/board_api.h"
#include <tusb.h>

#include <vector>
#include <map>
#include <list>
#include <string>

class HIDManagement
{
    public:
        
        bool initHID();
        void taskHID();

        bool saveMacrosDictionary(const char * const dataJSON);

        // Method to add intraprocess message queue with pushed switches
        void setMessageQueue(QueueHandle_t *_switchsPushQueue){ switchQUEUEInternal = _switchsPushQueue;};

        // Method to get intraprocess message queue with pushed switches
        QueueHandle_t *getMessageQueue() {return switchQUEUEInternal;};

        //Singletone class initialization
        static HIDManagement& getInstance()
        {
            if (instanceHID == nullptr)
            {
                instanceHID = new HIDManagement();
            }
            return *instanceHID;
        }

        //Delete copy class operators
        HIDManagement(const HIDManagement&) = delete;
        HIDManagement& operator=(const HIDManagement&) = delete;

        
    private:
        static HIDManagement* instanceHID;
        QueueHandle_t *switchQUEUEInternal = nullptr;
        TimerHandle_t USBTimerInform = nullptr;
        std::map<uint8_t, std::list<uint8_t>> macrosDictionary;
        std::vector<uint8_t> strokeQueue;

        HIDManagement(){};
        // Send action to host device (Keyboard, mouse, HID)
        void sendAction();

        friend void informTimerCB(TimerHandle_t xTimer);
        friend uint16_t tud_hid_get_report_cb(uint8_t instance, 
                                            uint8_t report_id, 
                                            hid_report_type_t report_type, 
                                            uint8_t* buffer, 
                                            uint16_t reqlen);
        friend void tud_hid_set_report_cb(uint8_t instance, 
                                        uint8_t report_id, 
                                        hid_report_type_t report_type, 
                                        uint8_t const* buffer, 
                                        uint16_t bufsize);
};

#endif