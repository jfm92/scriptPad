#include "filesManagement.h"
#include <stdio.h>
#include <bits/stdc++.h>
#include "pico/stdlib.h"

#include "libraries/cJSON/cJSON.h"

#include "f_util.h"
#include "ff.h"
#include "rtc.h"

#include "hw_config.h"

bool filesManagement::initFileManagement()
{
    bool result = false;

    sd_card_t *pSD = sd_get_by_num(0);
    if(pSD == NULL)
    {
        printf("PSD null\n");
        return result;
    }

    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if(FR_OK != fr)
    {
        printf("Error mounting SD card: %i\n", fr);
        return result;
    }

    printf("SD card mounted.\n");

    return result;
}

bool filesManagement::readFileContent(std::string filename, std::string *fileContent) 
{
    bool result = false;

    FIL *file; 
    FRESULT fr  = f_open(file, filename.c_str(), FA_READ);
    if (FR_OK != fr && FR_EXIST != fr) 
    {
        printf("File openning error\r\n");
        return result;
    }

    char buffer[100];

    while (f_gets(buffer, sizeof(buffer), file) != NULL) {
        *fileContent += buffer;
    }

    result = true;

    return result;
}

bool filesManagement::writeContentToFile(std::string filename, std::string fileContent)
{
    bool result = false;
    FIL *file;

    FRESULT fr = f_open(file, filename.c_str(), FA_OPEN_APPEND | FA_WRITE);
    if (FR_OK != fr && FR_EXIST != fr)
    {
        printf("Error openning\n");
        return result;
    }

    if (f_printf(file, fileContent.c_str()) < 0) {
        printf("f_printf failed\n");
        return result;
    }

    fr = f_close(file);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        return result;
    }

    result = true;
    return result;
}

bool filesManagement::deleteFile(std::string filename)
{
    bool result = false;
    if (remove(filename.c_str()) == 0)
    {
        printf("File %s deleted\r\n", filename.c_str());
        result = true;
    }

    return result;
}

void filesManagement::listFiles(std::list<std::string> *listName)
{
    FRESULT result;
    DIR directory;
    FILINFO fno;

    //Only root folder of the SD
    result = f_opendir(&directory, "/");
    if (result != FR_OK) 
    {
        printf("Error opnening dir\n");
    }

    while(1)
    {
        result = f_readdir(&directory, &fno);
        
        //Check if it's a file.
        //TODO: Only add JSON files
        if(fno.fattrib & AM_ARC)
        {
            printf("Adding file\n");
            listName->push_back(fno.fname);
        }

       if(result != FR_OK || fno.fname[0] == 0) break;
    }

    f_closedir(&directory);

    return;
}

void filesManagement::checkPreviousMacroUsed(std::string *fileName)
{
    std::list<std::string> filesList;
    listFiles(&filesList);

    auto foundPosition = std::find(filesList.begin(), filesList.end(), fileConfigName);
    if(foundPosition != filesList.end())
    {
        readFileContent(fileConfigName, fileName);
    }
}

void filesManagement::listMacroNames(std::string *fileContent, std::map<uint8_t, std::string> *macros, std::string *profileName)
{
    
    cJSON *dictionaryParsed = cJSON_Parse(fileContent->c_str());
    if(!dictionaryParsed)
    {
        printf("Error parsing dictonary JSON\n");

    }

    cJSON *nameJSON = cJSON_GetObjectItemCaseSensitive(dictionaryParsed, "name");
    *profileName = cJSON_Print(nameJSON);
    
    cJSON *switchMacro = NULL;
    cJSON *switchMacrosList = cJSON_GetObjectItemCaseSensitive(dictionaryParsed, "switchMacros");
    if(!switchMacrosList)
    {
        printf("Error Getting macro list from JSON\n");
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
        }

        // Get macroCodes to be used
        cJSON *macrosName = cJSON_GetObjectItemCaseSensitive(switchMacro, "macroName");
        if(!macrosName)
        {
            printf("Error getting macroCodes\n");
        }


        (*macros)[atoi(cJSON_Print(switchCode))] = cJSON_Print(macrosName);
        cJSON_free(switchCode);
        cJSON_free(macrosName);
    }

    cJSON_free(nameJSON);
    cJSON_free(switchMacro);
    cJSON_free(switchMacrosList);
    cJSON_free(dictionaryParsed);
}