#ifndef _GUI
#define _GUI

#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string>
#include <vector>
#include <algorithm>

class GUI
{
public:
    
    void setHIDQueue();
    void init();
    void LVGLTask();
    void setQueue(QueueHandle_t *_GUIPushQueue){GUIPushQueue = _GUIPushQueue;};
    QueueHandle_t *getQueue(){return GUIPushQueue;}
    void setGUIControlAvailable(bool _available){ controlActive = _available;};
    bool getGUIControlAvailable(){ return controlActive;};
    void GUIMoveTab(uint8_t tabNum);
    void setHIDTask(TaskHandle_t *_HIDTask_Handle){HIDTask_Handle = _HIDTask_Handle;};
    void setModemUSBTask(TaskHandle_t *_modemUSBTask_Handle){modemUSBTask_Handle = _modemUSBTask_Handle;};

    static GUI& getInstance()
        {
            if (instanceGUI == nullptr)
            {
                instanceGUI = new GUI();
            }
            return *instanceGUI;
        }

    //Delete copy class operators
    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

private:
    GUI(){};

    static GUI* instanceGUI;

    QueueHandle_t *GUIPushQueue;

    TaskHandle_t *HIDTask_Handle;
    TaskHandle_t *modemUSBTask_Handle;

    bool controlActive = true;

    lv_disp_draw_buf_t drawBuf1;
    lv_color_t buffer[320 * 10];
    lv_disp_drv_t dispDriver;
    lv_indev_drv_t inputDriver;
    lv_indev_t * inputDriverKeypad;

    uint16_t macroProfileIDSelected = 0;
    std::vector<std::string> macroProfileNameVector;

    void frontEndInit();
    void mainScreen();
    void webConfigScreen();
    void macrosScreen(std::string fileName);
    void loadMacroScreen(std::string profileName);
    void loadWebConfigScreen();

    void saveProfileSelected(std::string profileName){
        auto position = std::find(macroProfileNameVector.begin(), macroProfileNameVector.end(), profileName);

        // If on nay case we don't find it, we return position 0;
        macroProfileIDSelected = (position != macroProfileNameVector.end()) ? 
                                    std::distance(macroProfileNameVector.begin(), position) : 0;

    };

    std::string actualProfileID() {return macroProfileNameVector.empty() ? " " : macroProfileNameVector[macroProfileIDSelected];};

    std::string setNextProfile(){
        //We check if there a next profile, otherwise, we keep the actual ID
        macroProfileIDSelected = ((macroProfileIDSelected ++) <= macroProfileNameVector.size()) ? 
                                macroProfileIDSelected ++ : macroProfileIDSelected;

        return macroProfileNameVector[macroProfileIDSelected];
    }

    std::string getPreviousProfile(){
        //Check if we arrive to 0, otherwise we return the same previous
        macroProfileIDSelected = ((macroProfileIDSelected --) >= 0) ? 
                                macroProfileIDSelected -- : macroProfileIDSelected;

        return macroProfileNameVector[macroProfileIDSelected];
    }
    

    //Screens
    lv_obj_t *ui_mainScreen;
    lv_obj_t *ui_macrosScreen;
    lv_obj_t *ui_webConfigScreen;

    lv_obj_t *ui_optionsTab;
    lv_obj_t *ui_profileSelectorTab;
    lv_obj_t *ui_profilesRoller;
    lv_obj_t *ui_webConfigTab;
    lv_obj_t *ui_webConfigBtn;
    
    lv_obj_t *ui_macrosTab;
    lv_obj_t *ui_Profile_Tab;
    lv_obj_t *ui_Panel2;
    lv_obj_t *ui_Label2;
    lv_obj_t *ui_Panel1;
    lv_obj_t *ui_Label1;
    lv_obj_t *ui_Panel3;
    lv_obj_t *ui_Label3;
    lv_obj_t *ui_Panel4;
    lv_obj_t *ui_Label4;
    lv_obj_t *ui_Panel5;
    lv_obj_t *ui_Label5;
    lv_obj_t *ui_Panel6;
    lv_obj_t *ui_Label6;
    lv_obj_t *ui_Panel7;
    lv_obj_t *ui_Label7;
    lv_obj_t *ui_Panel8;
    lv_obj_t *ui_Label8;
    lv_obj_t *ui_Panel9;
    lv_obj_t *ui_Label9;
    lv_obj_t *ui_Panel10;
    lv_obj_t *ui_Label11;
    lv_obj_t *ui_Panel11;
    lv_obj_t *ui_Label12;
    lv_obj_t *ui_Panel12;
    lv_obj_t *ui_Label13;
    
    lv_obj_t *ui_TabView4;
    lv_obj_t *ui_Button4;
    lv_obj_t *ui_Label10;
    lv_obj_t *ui_TextArea1;
    lv_obj_t *ui____initial_actions0;
    lv_group_t * group_interact;

    friend void ui_event_Roller1( lv_event_t * e);
    friend void ui_event_Button3( lv_event_t * e);
    friend void ui_event_Button4( lv_event_t * e);
};



#endif