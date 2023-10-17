#include "filesManagement.h"

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