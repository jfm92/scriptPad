#include "HIDManagement.h"
#include "libraries/cJSON/cJSON.h"

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
}

void informTimerCB(TimerHandle_t xTimer)
{
    HIDManagement& managementHID = HIDManagement::getInstance();
    managementHID.sendAction();
}

bool HIDManagement::initHID()
{
    bool initCompleted = false;
    strokeQueue.clear();

    if(!tusb_init())
    {
        printf("tusb_init init failed\r\n");
        return initCompleted;
    }

    USBTimerInform = xTimerCreate("USBInformTimer", pdMS_TO_TICKS(1), pdTRUE, 0, &informTimerCB);
    if (!USBTimerInform) {
        printf("USBInformTimer init failed\r\n");
        return initCompleted;
    }
    
    if (xTimerStart(USBTimerInform, 0) != pdPASS) {
        printf("USBInformTimer start failed\r\n");
        return initCompleted;
    }

    printf("HID init complete\r\n");
    initCompleted = true;

    return initCompleted;
}

void HIDManagement::taskHID()
{
    tud_task();

    uint8_t received_message;
    if (xQueueReceive(*switchQUEUEInternal, &received_message, 0)) {
        strokeQueue.push_back(received_message); //Save received keys to the internal vector queue
    }
}

bool HIDManagement::saveMacrosDictionary(const char * const dataJSON)
{
    bool succeed = false;
    cJSON *codeKey;
    cJSON *macroCodes;
    cJSON *switchCode;

    cJSON *dictionaryParsed = cJSON_Parse(dataJSON);
    if(!dictionaryParsed)
    {
        printf("Error parsing dictonary JSON\n");
        return succeed;
    }

    cJSON *switchMacro = NULL;
    cJSON *switchMacrosList = cJSON_GetObjectItemCaseSensitive(dictionaryParsed, "switchMacros");
    if(!switchMacrosList)
    {
        printf("Error Getting macro list from JSON\n");
        return succeed;
    }

    //Iterates over all switch list to get macro associated to each switch
    cJSON_ArrayForEach(switchMacro, switchMacrosList)
    {
        std::list<uint8_t> macrosCodeList;

        //Get switch code associated.
        cJSON *switchCode = cJSON_GetObjectItemCaseSensitive(switchMacro, "switchCode");
        if(!switchCode)
        {
            printf("Error getting switchCode\n");
            return succeed;
        }

        // Get macroCodes to be used
        macroCodes = cJSON_GetObjectItemCaseSensitive(switchMacro, "macroList");
        if(!macroCodes)
        {
            printf("Error getting macroCodes\n");
            return succeed;
        }

        for(int i = 0; i < cJSON_GetArraySize(macroCodes); ++i)
        {
            //Get each element of the array
            codeKey = cJSON_GetArrayItem(macroCodes, i);
            macrosCodeList.push_back(atoi(cJSON_Print(codeKey)));
           // cJSON_free(codeKey);
        }

        //Asociate each switchCode with macroCodeList
        macrosDictionary[atoi(cJSON_Print(switchCode))] = macrosCodeList;
    }

    cJSON_Delete(dictionaryParsed);

    succeed = true;

    return succeed;
}

void HIDManagement::sendAction()
{
    bool pendingStrokes = !strokeQueue.empty();
    static bool has_keyboard_key = false;
    
    if(tud_suspended() && pendingStrokes)
    {
        //Send wakeup request if connection was suspendend
        tud_remote_wakeup();
        return;
    }

    //If device is not ready, we don't send anything and discard data
    if(!tud_hid_ready()) return;

    if(pendingStrokes)
    {
        uint8_t keycodeArray[6] = { 0 };
        uint8_t keyArrayIndex = 0;
        bool previousSend = false;

        //Get last keyStroke
        uint8_t switchNum = strokeQueue.back();
        strokeQueue.pop_back();
        
        //This loops iterates over the keys list associated to switch
        for(auto keyCode : macrosDictionary[switchNum])
        {
            keycodeArray[keyArrayIndex] = keyCode;
            ++ keyArrayIndex; 

            //This is a limitation of the tinyUSB library that only allows to send 6 elements on each inform
            if(keyArrayIndex > 5)
            {
                if(previousSend)
                {
                    // If we already send an inform with information, we need to send a empty one;
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    tud_hid_keyboard_report(1, 0, NULL); 
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
                tud_hid_keyboard_report(1, 0, keycodeArray); //TODO: CHange 1 to KEYBOARD code
                previousSend = true;

                keyArrayIndex = 0;
                memset(keycodeArray, 0, sizeof(keycodeArray));
            }
        }
        
        //Sending Leftovers
        if(keyArrayIndex != 0)
        {
            if(previousSend)
            {
                // If we already send an inform with information, we need to send a empty one;
                vTaskDelay(10 / portTICK_PERIOD_MS);
                tud_hid_keyboard_report(1, 0, NULL); 
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            tud_hid_keyboard_report(1, 0, keycodeArray);
        }

        has_keyboard_key = true;
    }
    else
    {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_keyboard_report(1, 0, NULL); //TODO: Change report ID to keyboard
        has_keyboard_key = false;
    }
}

