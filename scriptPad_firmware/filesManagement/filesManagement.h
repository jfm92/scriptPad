#ifndef FILES_MANAGEMENT
#define FILES_MANAGEMENT

#define ROOT_SIZE 0x100000
#define ROOT_OFFSET 0x100000

#include <pfs.h>
#include <lfs.h>
#include <dirent.h>

#include <string>
#include <list>

class filesManagement
{
    public:
        
        //Initialize filesystem and mount volume
        void initFileManagement();
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
        struct pfs_pfs *pfs;
        struct lfs_config cfg;

        static filesManagement* instanceFileManagement;

        filesManagement(){};
};

#endif