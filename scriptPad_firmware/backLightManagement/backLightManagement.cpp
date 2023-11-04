#include "backLightManagement.h"



void backlightmanagement::init()
{
    ledMatrix = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, pinNumber, ledNum, PicoLed::FORMAT_GRB);
    ledMatrix.setBrightness(50);
}

void backlightmanagement::setBackgroundEffect(uint8_t backgroundEffect)
{
    if(backgroundEffect == 0x00)
    {
        rainbowPalette.push_back( PicoLed::RGB(255, 0, 0) );
        rainbowPalette.push_back( PicoLed::RGB(255, 255, 0) );
        rainbowPalette.push_back( PicoLed::RGB(0, 255, 0) );
        rainbowPalette.push_back( PicoLed::RGB(0, 255, 255) );
        rainbowPalette.push_back( PicoLed::RGB(0, 0, 255) );
        rainbowPalette.push_back( PicoLed::RGB(255, 0, 255) );
        rainbowPalette.push_back( PicoLed::RGB(255, 255, 255) );
        rainbowPalette.push_back( PicoLed::RGB(255, 0, 125) );
    }
    else if(backgroundEffect == 0x01)
    {
        firePalette.push_back( PicoLed::RGB(0, 0, 0) );
        firePalette.push_back( PicoLed::RGB(255, 0, 0) );
        firePalette.push_back( PicoLed::RGB(255, 255, 0) );
        firePalette.push_back( PicoLed::RGB(255, 255, 255) );
    }
    else if(backgroundEffect == 0x02)
    {

    }
    effectSelected = backgroundEffect;

}
    

void backlightmanagement::backGroundLightTask()
{
    PicoLed::Marquee effectMarquee(ledMatrix, rainbowPalette, 5.0, -2.0, 1.0);


    while(1)
    {
        if(effectSelected == 0x00)
        {
            if (effectReset) {
                effectReset = false;
                effectMarquee.reset();
            }
            if (effectMarquee.animate()) {
                ledMatrix.show();
            }
        }
        else if (effectSelected == 0x01)
        {

        }
            
        
        sleep_ms(5);
        // Check for effect change
        timeNow = to_ms_since_boot( get_absolute_time() );
        if (timeNow > timeNext) {
            timeNext = timeNow + effectChangeInterval;
            effectIndex = (effectIndex + 1) % effectCount;
            effectReset = true;
        }

    }
        
}