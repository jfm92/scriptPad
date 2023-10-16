#ifndef HID_MANAGEMENT
#define HID_MANAGEMENT

#include "FreeRTOS.h"
#include "queue.h"
#include <timers.h>

#include "bsp/board.h"
#include <tusb.h>

#include <vector>

class HIDManagement
{
    public:
        HIDManagement(QueueHandle_t *_switchQUEUEInternal) : switchQUEUEInternal(_switchQUEUEInternal) {};
        bool initHID();
        void taskHID();
        
    private:
        QueueHandle_t *switchQUEUEInternal = nullptr;
        TimerHandle_t USBTimerInform = nullptr;
        

        friend uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen);
        friend void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize);
};

#endif