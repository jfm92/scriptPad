#include "GUI.h"

#include "../filesManagement/filesManagement.h"
#include "../HIDManagement/HIDManagement.h"
#include "drivers/ST7789V/ST7789V.h"
#include "../usb_descriptor.h"
#include "hardware/watchdog.h"

#include "FreeRTOS.h"
#include "queue.h"
#include <timers.h>

#include "stdio.h"
#include <list>

static lv_color_t *bufferPri;
static lv_color_t *bufferSec;
static lv_color_t *bufferInUse;
static uint8_t bufferNum = 1;

/********************** LVGL callbacks*****************************************/
static void displayFlush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    ST7789V& display = ST7789V::getInstance();

    display.draw(area->x1,
            area->y1,               
            area->x2 +1 ,   
            area->y2 +1 , 
            (uint8_t *)color_p);

    lv_disp_flush_ready(disp_drv);
}

static uint8_t state = 0;
static uint32_t keyPrev = 0;
static bool keyPressed = 0;
static void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{ 
    uint8_t recvSwitchCode;
    GUI& GUIInstance = GUI::getInstance();
    bool controlAvailable = GUIInstance.getGUIControlAvailable();

    if(controlAvailable && xQueueReceive(*GUIInstance.getQueue(), &recvSwitchCode, 0))
    {
        //TODO: Maybe change to a map??
        switch (recvSwitchCode)
        {
            case 2:
                keyPrev = LV_KEY_UP;
                break;
            
            case 5:
                state = !state;
                GUIInstance.GUIMoveTab(state);
                break;
            
            case 6:
                keyPrev = LV_KEY_ENTER;
                break;
            
            case 7:
                state = !state;
                GUIInstance.GUIMoveTab(state);
                break;
            
            case 10:
                keyPrev = LV_KEY_DOWN;
                break;
            
            default:
                break;
        }

        keyPressed = true;
    }

    data->key = keyPrev; 

    if(keyPressed)
    {
        data->state = LV_INDEV_STATE_PRESSED;
        keyPressed=!keyPressed;
    }
    else data->state = LV_INDEV_STATE_RELEASED;
}

void ui_event_Roller1( lv_event_t * e) {
    GUI& GUIInstance = GUI::getInstance();
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
 
    if ( event_code == LV_EVENT_CLICKED) {
        char name[256];
        lv_roller_get_selected_str(target, name, 256);
        GUIInstance.loadMacroScreen(name);
    }
}

void ui_event_Button3( lv_event_t * e) {
    GUI& GUIInstance = GUI::getInstance();
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if ( event_code == LV_EVENT_CLICKED) {
        GUIInstance.loadWebConfigScreen();
    }
}

void ui_event_Button4( lv_event_t * e) {
    GUI& GUIInstance = GUI::getInstance();
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if ( event_code == LV_EVENT_CLICKED) {
        watchdog_enable(0, 1);
    }
}

void lvglTick(void* param)
{
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(50));
        lv_tick_inc(50);
    }
}

/**********************Public functions*****************************************/

void GUI::init()
{
    lv_init();
    static lv_color_t *buffer0 = new lv_color_t[320*20];

    memset(buffer0, 0x00, 320*20);
    
    lv_disp_draw_buf_init(&drawBuf1, buffer0, NULL, 320 * 20);
    lv_disp_drv_init(&dispDriver);

    dispDriver.hor_res = 320;
    dispDriver.ver_res = 240;

    dispDriver.flush_cb = displayFlush;
    dispDriver.draw_buf = &drawBuf1;  
    lv_disp_drv_register(&dispDriver);

    lv_indev_drv_init(&inputDriver);
    inputDriver.type = LV_INDEV_TYPE_KEYPAD;
    inputDriver.read_cb = my_input_read;
    inputDriverKeypad = lv_indev_drv_register(&inputDriver);
    group_interact = lv_group_create();
    lv_indev_set_group(inputDriverKeypad, group_interact);

    frontEndInit();

    xTaskCreate(lvglTick, "TCI", 256, NULL,configMAX_PRIORITIES-1 , NULL);    
}

void GUI::frontEndInit()
{
    lv_disp_t *display = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_INDIGO), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(display, theme);

    mainScreen();
    //macrosScreen("profile1");

    ui____initial_actions0 = lv_obj_create(NULL);

    lv_disp_load_scr(ui_mainScreen);
}

void GUI::LVGLTask()
{
    lv_timer_handler();
}

void GUI::GUIMoveTab(uint8_t tabNum)
{
    lv_tabview_set_act(ui_optionsTab, tabNum, LV_ANIM_ON);
    lv_group_focus_obj((tabNum) ? ui_webConfigBtn : ui_profilesRoller);
}

void GUI::mainScreen(void)
{
    std::list<std::string> filesList;
    std::string fileListNames;

    printf("Creating main\n");
    ui_mainScreen = lv_obj_create(NULL);
    lv_obj_clear_flag( ui_mainScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

    ui_optionsTab = lv_tabview_create(ui_mainScreen, LV_DIR_TOP, 50);
    lv_obj_set_width( ui_optionsTab, 320);
    lv_obj_set_height( ui_optionsTab, 240);
    lv_obj_set_align( ui_optionsTab, LV_ALIGN_TOP_MID );
    lv_obj_clear_flag( ui_optionsTab, LV_OBJ_FLAG_SCROLLABLE );

    ui_profileSelectorTab = lv_tabview_add_tab(ui_optionsTab, "Profiles");
    
    filesManagement& filesControl = filesManagement::getInstance();
    filesControl.listFiles(&filesList);

    for(auto fileName : filesList)
    {
        fileListNames += fileName + '\n';
    }

    ui_profilesRoller = lv_roller_create(ui_profileSelectorTab);
    lv_roller_set_options( ui_profilesRoller, fileListNames.c_str(), LV_ROLLER_MODE_NORMAL );
    lv_obj_set_width( ui_profilesRoller, 200);
    lv_obj_set_height( ui_profilesRoller, 150);
    lv_obj_set_align( ui_profilesRoller, LV_ALIGN_TOP_MID );

    lv_group_add_obj(group_interact, ui_profilesRoller);

    ui_webConfigTab = lv_tabview_add_tab(ui_optionsTab, "Web Config");

    ui_webConfigBtn = lv_btn_create(ui_webConfigTab);
    lv_obj_set_width( ui_webConfigBtn, 100);
    lv_obj_set_height( ui_webConfigBtn, 50);
    lv_obj_set_align( ui_webConfigBtn, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_webConfigBtn, LV_OBJ_FLAG_SCROLL_ON_FOCUS );
    lv_obj_add_flag( ui_webConfigBtn, LV_OBJ_FLAG_CLICKABLE );
    lv_obj_clear_flag( ui_webConfigBtn, LV_OBJ_FLAG_SCROLLABLE ); 

    lv_obj_t *webConfigLabel = lv_label_create(ui_webConfigBtn);
    lv_label_set_text(webConfigLabel, "Configuration");
    lv_label_set_long_mode(webConfigLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(webConfigLabel, LV_ALIGN_CENTER, 0, 0);

    lv_group_add_obj(group_interact, ui_webConfigBtn);

    lv_obj_add_event_cb(ui_profilesRoller, &ui_event_Roller1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_webConfigBtn, &ui_event_Button3, LV_EVENT_ALL, NULL);

}

void GUI::webConfigScreen()
{
    ui_webConfigScreen = lv_obj_create(NULL);
    lv_obj_clear_flag( ui_webConfigScreen, LV_OBJ_FLAG_SCROLLABLE );

    ui_TabView4 = lv_tabview_create(ui_webConfigScreen, LV_DIR_TOP, 25);
    lv_obj_set_width( ui_TabView4, 320);
    lv_obj_set_height( ui_TabView4, 240);
    lv_obj_set_align( ui_TabView4, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_TabView4, LV_OBJ_FLAG_SCROLLABLE );


    ui_webConfigTab = lv_tabview_add_tab(ui_TabView4, "webConfig");

    ui_Button4 = lv_btn_create(ui_webConfigTab);
    lv_obj_set_width( ui_Button4, 150);
    lv_obj_set_height( ui_Button4, 50);
    lv_obj_set_align( ui_Button4, LV_ALIGN_BOTTOM_MID );
    lv_obj_add_flag( ui_Button4, LV_OBJ_FLAG_SCROLL_ON_FOCUS );
    lv_obj_clear_flag( ui_Button4, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_style_bg_color(ui_Button4, lv_color_hex(0xCA2222), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_Button4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_group_add_obj(group_interact, ui_Button4);

    ui_Label10 = lv_label_create(ui_Button4);
    lv_obj_set_width( ui_Label10, LV_SIZE_CONTENT);
    lv_obj_set_height( ui_Label10, LV_SIZE_CONTENT);
    lv_obj_set_align( ui_Label10, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Label10,"Close config mode");

    ui_TextArea1 = lv_textarea_create(ui_webConfigTab);
    lv_obj_set_width( ui_TextArea1, 230);
    lv_obj_set_height( ui_TextArea1, LV_SIZE_CONTENT);   /// 50
    lv_obj_set_align( ui_TextArea1, LV_ALIGN_TOP_MID );
    lv_textarea_set_text(ui_TextArea1,"Open your browser and type IP:\n192.168.7.1");
    lv_textarea_set_placeholder_text(ui_TextArea1,"Placeholder...");
    lv_obj_set_style_text_align(ui_TextArea1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Button4, &ui_event_Button4, LV_EVENT_ALL, NULL);

}

void GUI::macrosScreen(std::string fileName)
{
    bool succed = false;
    std::string fileContent;
    std::string profileName;
    std::map<uint8_t, std::string> macroList;

    filesManagement& FSmanagementInstance = filesManagement::getInstance();
    HIDManagement& managementHID = HIDManagement::getInstance();

    succed = FSmanagementInstance.readFileContent(fileName, &fileContent);
    if(!succed)
    {
        //Return to previous main window
    }

    FSmanagementInstance.listMacroNames(&fileContent, &macroList, &profileName);
    if(macroList.empty())
    {
        //Somenthing goes wrong, and it's empty. Return
    }

    //Save this new macro list to HID
    succed = managementHID.saveMacrosDictionary(fileContent.c_str());
    if(!succed)
    {
        //Somenthing went wrong saving this content into the HID
    }

    //Everything is ready, time to paint teh GUI
    ui_macrosScreen = lv_obj_create(NULL);
    lv_obj_clear_flag( ui_macrosScreen, LV_OBJ_FLAG_SCROLLABLE );

    ui_macrosTab = lv_tabview_create(ui_macrosScreen, LV_DIR_TOP, 50);
    lv_obj_set_width( ui_macrosTab, 320);
    lv_obj_set_height( ui_macrosTab, 240);
    lv_obj_set_align( ui_macrosTab, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_macrosTab, LV_OBJ_FLAG_SCROLLABLE ); 

    ui_Profile_Tab = lv_tabview_add_tab(ui_macrosTab, profileName.c_str());
    ui_Panel2 = lv_obj_create(ui_Profile_Tab);
    lv_obj_set_width( ui_Panel2, 80);
    lv_obj_set_height( ui_Panel2, 35);
    lv_obj_clear_flag( ui_Panel2, LV_OBJ_FLAG_SCROLLABLE );

    //This build macro screen
    for(auto macro :macroList )
    {
        lv_obj_t * macroButton = lv_btn_create(ui_Profile_Tab);
        
        //Set exact position for each button
        switch ( macro.first)
        {
            case 0:
                lv_obj_align(macroButton, LV_ALIGN_TOP_LEFT, 0, -10);
            break;
            case 1:
                lv_obj_align(macroButton, LV_ALIGN_TOP_MID, -40, -10);
            break;
            case 2:
                lv_obj_align(macroButton, LV_ALIGN_TOP_MID, 40, -10);
            break;
            case 3:
                lv_obj_align(macroButton, LV_ALIGN_TOP_RIGHT, 0, -10);
            break;
            case 4:
                lv_obj_align(macroButton, LV_ALIGN_LEFT_MID, 0, 0);
            break;
            case 5:
                lv_obj_align(macroButton, LV_ALIGN_CENTER, -40, 0);
            break;
            case 6:
                lv_obj_align(macroButton, LV_ALIGN_CENTER, 40, 0);
            break;
            case 7:
                lv_obj_align(macroButton, LV_ALIGN_RIGHT_MID, 0, 0);
            break;
            case 8:
                lv_obj_align(macroButton, LV_ALIGN_BOTTOM_LEFT, 0, 10);
            break;
            case 9:
                lv_obj_align(macroButton, LV_ALIGN_BOTTOM_MID, -40, 10);
            break;
            case 10:
                lv_obj_align(macroButton, LV_ALIGN_BOTTOM_MID, 40, 10);
            break;
            case 11:
                lv_obj_align(macroButton, LV_ALIGN_BOTTOM_RIGHT, 0, 10);
            break;
            default:
            break;
        }

        lv_obj_set_size(macroButton,55,55);

        lv_obj_t * label = lv_label_create(macroButton);
        lv_label_set_text(label, macro.second.c_str());
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_width(label, 55);
        lv_obj_align(label, LV_ALIGN_CENTER, 12, 0); //Necessary to apply this offset
    }
}

void GUI::loadMacroScreen(std::string profileName)
{
    Foo(false);
    setGUIControlAvailable(false);
    macrosScreen(profileName);
    lv_scr_load_anim(ui_macrosScreen, LV_SCR_LOAD_ANIM_OVER_TOP, 50, 0, true);
    vTaskResume(*HIDTask_Handle);
}

void GUI::loadWebConfigScreen()
{
    Foo(true);
    webConfigScreen();
    lv_scr_load_anim(ui_webConfigScreen, LV_SCR_LOAD_ANIM_OVER_TOP, 50, 0, true);
    vTaskResume(*modemUSBTask_Handle);
}
