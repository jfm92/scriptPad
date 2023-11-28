#include "encoderManagement.h"

#include "stdio.h"

////////////////////// Externals callbacks //////////////////////
void encoderButtonCB(uint gpio, uint32_t events);
uint8_t pushedTimes = 0;

int64_t longPressTimerCB(alarm_id_t id, void *user_data){
    encoderManagement& encoderManagementInstance = encoderManagement::getInstance();

    if(encoderManagementInstance.getEnconderSwitchState()){
        if(encoderManagementInstance.isLongPush()){
            //TODO: Send long push notification.
            printf("Long press\n");
            encoderManagementInstance.resetPushCycle();

            //If the user forget the finger on the switch, we need to wait until it's released
            while(encoderManagementInstance.getEnconderSwitchState()){}

            gpio_set_irq_enabled_with_callback(encoderManagementInstance.getFunctionGPIO(0), GPIO_IRQ_EDGE_FALL , true, &encoderButtonCB);
        }

        //This could be al long push, but we need to wait a few cycles more
        encoderManagementInstance.increasePushCycle();
        add_alarm_in_ms(250, longPressTimerCB, NULL, false);
    }
    else{
        // Not a long push, just a normal one, reset counter and reenable IRQ
        //TODO: Send normal push notication
        printf("Pushed\n");
        encoderManagementInstance.resetPushCycle();
        gpio_set_irq_enabled_with_callback(encoderManagementInstance.getFunctionGPIO(0), GPIO_IRQ_EDGE_FALL , true, &encoderButtonCB);
    }

    return 0;
}

void encoderButtonCB(uint gpio, uint32_t events){
    // Switch was pushed, but we need to be sure it's not a long push, push notification will be send
    // on timerCB
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL , false, &encoderButtonCB);
    add_alarm_in_ms(50, longPressTimerCB, NULL, false);
}

void encoderTask(void *param){
    encoderManagement& encoderManagementInstance = encoderManagement::getInstance();

    uint8_t rightCount = 0;
    uint8_t leftCount = 0;

    uint8_t previousState = encoderManagementInstance.getEnconderContactAState();

    //  This loop is basically to check if we change from contact A to B or viceversa
    // but due to it could be some bouncing between contacts, we send movement if
    // we detect contact change two times.
    while(1){
        uint8_t actualState = encoderManagementInstance.getEnconderContactAState();
        if(rightCount == 2){
            printf("Right\n");
            rightCount= 0; 
        }
        else if(leftCount == 2) {
            // TODO: Send left movement to queue
            printf("Left\n");
            leftCount = 0;
        }

        if(actualState != previousState){
            if(actualState != encoderManagementInstance.getEnconderContactBState()){
                rightCount = (rightCount > 2) ? rightCount : rightCount + 1;
                leftCount = (leftCount == 0) ? leftCount : leftCount -1;
            }
            else{
                leftCount = (leftCount > 2) ? leftCount : leftCount + 1;
                rightCount = (rightCount == 0) ? rightCount : rightCount -1;
            }
            previousState = actualState;
        }
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
        // Position 1 and 2, are encoder contacts
        // This will be checked on a thread, so we don't need an interruption
        if(gpio.first > 0 && gpio.first < 3){ 
            gpio_init(gpio.second);
            gpio_set_pulls(gpio.second, true, false); //PullUP
            gpio_set_dir(gpio.second, false);
        }
        else if(gpio.first == 0){
            gpio_init(gpio.second);
            gpio_set_pulls(gpio.second, true, false);
            gpio_set_irq_enabled_with_callback(gpio.second, GPIO_IRQ_EDGE_FALL, true, &encoderButtonCB);
        }
    }

    xTaskCreate( encoderTask, "encoderControl", 4096, NULL, configMAX_PRIORITIES-2, &encoderTask_Handle);

    result = (encoderTask_Handle != nullptr) ? true : false;

    return result;
}