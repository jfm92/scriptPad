#include "filesManagement.h"
#include "libraries/cJSON/cJSON.h"
#include <bits/stdc++.h>

void filesManagement::initFileManagement()
{
    ffs_pico_createcfg (&cfg, ROOT_OFFSET, ROOT_SIZE);
    pfs = pfs_ffs_create (&cfg);
    pfs_mount (pfs, "/");
}

bool filesManagement::readFileContent(std::string filename, std::string *fileContent) 
{
    bool result = false;

    FILE * file = fopen(filename.c_str(), "r");
    if (file == NULL) 
    {
        printf("File openning error\r\n");
        return result;
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        *fileContent += buffer;
    }

    result = true;

    return result;
}

bool filesManagement::createFile(std::string filename)
{
    bool result = false;

    FILE *fd = fopen (filename.c_str() , "w");
    if(fd != NULL)
    {
        printf("File %s created.\n", filename.c_str());
        fclose (fd);

        result = true;
    }
    
    return result;
}

bool filesManagement::writeContentToFile(std::string filename, std::string fileContent)
{
    bool result = false;
    FILE* file = fopen(filename.c_str(), "w");

    if (file)
    {
        fprintf(file, fileContent.c_str());
        fclose(file);
        result = true;
    }

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
    DIR *fsDirectory;
    struct dirent *fsEntry;

    fsDirectory = opendir("/");
    if(fsDirectory == NULL)
    {
        //This will return 0 elements
        printf("Error opening directory");
        return;
    }

    while((fsEntry = readdir(fsDirectory)) != NULL)
    {
        std::string auxFileName(fsEntry->d_name);
 
        if(!auxFileName.empty())
        {
            listName->push_back(auxFileName);
        }
    }

    closedir(fsDirectory);

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