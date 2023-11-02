#ifndef FILES_MANAGEMENT
#define FILES_MANAGEMENT

#define ROOT_SIZE 0x100000
#define ROOT_OFFSET 0x100000

/*
#include <pfs.h>
#include <lfs.h>
#include <dirent.h>*/

#include <string>
#include <list>
#include <map>

class filesManagement
{
    public:
        
        //Initialize filesystem and mount volume
        bool initFileManagement();
        //Try to open a file and if it exist and it's valid it returns file content
        bool readFileContent(std::string filename, std::string *fileContent);
        //Create a new file. Returns true if it was create.
        bool createFile(std::string filename);
        //Write content to a give file.
        bool writeContentToFile(std::string filename, std::string fileContent);
        //Delete a file giving the name
        bool deleteFile(std::string filename);
        //Return a list of files available on fs
        void listFiles(std::list<std::string> *listName);
        // Check if a macro config was set previously and save by reference the name
        void checkPreviousMacroUsed(std::string *fileName);
        // Get configured file for save config
        void getConfigFileName(std::string *fileName) {*fileName = fileConfigName;};
        // Set a new name config save file
        void setConfigFileName(std::string *fileName) {fileConfigName = *fileName;};
        //Return a map with macro name and ID
        void listMacroNames(std::string *fileContent, std::map<uint8_t, std::string> *macros,  std::string *profileName);

        //Singletone class initialization
        static filesManagement& getInstance()
        {
            if (instanceFileManagement == nullptr)
            {
                instanceFileManagement = new filesManagement();
            }
            return *instanceFileManagement;
        }

        //Delete copy class operators
        filesManagement(const filesManagement&) = delete;
        filesManagement& operator=(const filesManagement&) = delete;

    private:

        static filesManagement* instanceFileManagement;

        std::string fileConfigName = "savedConfig.cfg";

        filesManagement(){};
};

#endif