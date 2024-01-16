#include "encoderManagement.h"

#include "stdio.h"

#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

////////////////////// Externals callbacks //////////////////////
void encoderButtonCB(uint gpio, uint32_t events);
uint8_t pushedTimes = 0;
BaseType_t xHigherPrioritTaskWoken = pdFALSE;

int64_t longPressTimerCB(alarm_id_t id, void *user_data){
    encoderManagement& encoderManagementInstance = encoderManagement::getInstance();

    uint8_t encoderSWPin = encoderManagementInstance.encoderGPIOList[0];

    if(encoderManagementInstance.getEnconderSwitchState()){
        if(encoderManagementInstance.isLongPush()){
            //TODO: Send long push notification.
            printf("Long press\n");
            encoderManagementInstance.resetPushCycle();

            //If the user forget the finger on the switch, we need to wait until it's released
            while(encoderManagementInstance.getEnconderSwitchState()){}

            // We send the same ID but in a different queue
            uint8_t ID = encoderManagementInstance.encoderGPIOSWID[0];
            xQueueSendToFrontFromISR(*encoderManagementInstance.GUIQUEUEInternal, &ID, &xHigherPrioritTaskWoken );

            gpio_set_irq_enabled(encoderSWPin, GPIO_IRQ_EDGE_FALL, true); 
        }

        //This could be al long push, but we need to wait a few cycles more
        encoderManagementInstance.increasePushCycle();
        add_alarm_in_ms(250, longPressTimerCB, NULL, false);
    }
    else{
        // Not a long push, just a normal one, reset counter and reenable IRQ
        printf("Encoder Pushed\n");

        uint8_t ID = encoderManagementInstance.encoderGPIOSWID[0]; //ID of encoder on clockwise movement
        xQueueSendToFrontFromISR(*encoderManagementInstance.encoderQUEUEInternal, &ID, &xHigherPrioritTaskWoken );

        encoderManagementInstance.resetPushCycle();
        gpio_set_irq_enabled(encoderSWPin, GPIO_IRQ_EDGE_FALL, true); 
    }

    return 0;
}

void encoderCB(void){
    encoderManagement& encoderManagementInstance = encoderManagement::getInstance();

    // Are selected by their pin ID
    uint8_t encoderAPin = encoderManagementInstance.encoderGPIOList[1];
    uint8_t encoderBPin = encoderManagementInstance.encoderGPIOList[2];
    uint8_t encoderSWPin = encoderManagementInstance.encoderGPIOList[0];


    //Get which pin produced interruption
    bool encoderAInt = gpio_get_irq_event_mask(encoderAPin) & GPIO_IRQ_EDGE_FALL;
    bool encoderBInt = gpio_get_irq_event_mask(encoderBPin) & GPIO_IRQ_EDGE_FALL;
    bool encoderSWInt = gpio_get_irq_event_mask(encoderSWPin) & GPIO_IRQ_EDGE_FALL;

    if(encoderAInt || encoderBInt){
        gpio_acknowledge_irq(encoderAInt ? encoderAPin : encoderBPin, GPIO_IRQ_EDGE_FALL); //Ack Int

        if (time_us_64() > encoderManagementInstance.timeCounter + encoderManagementInstance.timeThreshold)
        {
            //Base on which contact was activated, we need to do a different comparasion
            bool encoderCondition = encoderAInt ? 
            (encoderManagementInstance.getEnconderContactAState() == encoderManagementInstance.getEnconderContactBState()) : 
            (encoderManagementInstance.getEnconderContactAState() != encoderManagementInstance.getEnconderContactBState());

            if (encoderCondition)
            {
                encoderManagementInstance.ISRCounterA++;
                if(encoderManagementInstance.ISRCounterA == encoderManagementInstance.minIntCount){
                    printf("Anti Clockwise Encoder\n");
                    
                    encoderManagementInstance.ISRCounterA = 0;
                    encoderManagementInstance.ISRCounterB = 0;

                    uint8_t ID = encoderManagementInstance.encoderGPIOSWID[1]; //ID of encoder on anti clockwise movement
                    xQueueSendToFrontFromISR(*encoderManagementInstance.encoderQUEUEInternal, &ID, &xHigherPrioritTaskWoken );
                }
            }
            else
            {
                encoderManagementInstance.ISRCounterB++;
                if(encoderManagementInstance.ISRCounterB == encoderManagementInstance.minIntCount){
                    printf("Clockwise Encoder\n");

                    encoderManagementInstance.ISRCounterB = 0;
                    encoderManagementInstance.ISRCounterA = 0;

                    uint8_t ID = encoderManagementInstance.encoderGPIOSWID[2]; //ID of encoder on clockwise movement
                    xQueueSendToFrontFromISR(*encoderManagementInstance.encoderQUEUEInternal, &ID, &xHigherPrioritTaskWoken );
                }
            }
            encoderManagementInstance.timeCounter = time_us_64();
        }

    }

    // Switch encoder management
    if (encoderSWInt) {
        gpio_acknowledge_irq(encoderSWPin, GPIO_IRQ_EDGE_FALL);
        gpio_set_irq_enabled(encoderSWPin, GPIO_IRQ_EDGE_FALL, false); 

        //Disabled to avoid bouncing
        add_alarm_in_ms(150, longPressTimerCB, NULL, false);
    }
}


////////////////////// Externals callbacks End //////////////////////

bool encoderManagement::init(){

    bool result = false;

    if(encoderInitialized || 
        !gpioEncoder || 
        !encoderQUEUEInternal || 
        !GUIQUEUEInternal){
            return result;
        }
    

    //Initialize each GPIO of the encoder
    for(const auto& gpio : *gpioEncoder){
        gpio_init(gpio.second);
        gpio_set_pulls(gpio.second, true, false);//PullUp
        gpio_set_dir(gpio.second, false); //Input
        gpio_set_irq_enabled(gpio.second, GPIO_IRQ_EDGE_FALL, true); 

        gpio_add_raw_irq_handler(gpio.second, encoderCB);

        encoderGPIOList.push_back(gpio.second);
        encoderGPIOSWID.push_back(gpio.first);
    }
    
    
    irq_set_enabled(IO_IRQ_BANK0, true);

    result = (encoderTask_Handle != nullptr) ? true : false;

    return result;
}