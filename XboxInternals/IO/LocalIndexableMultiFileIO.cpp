#include "LocalIndexableMultiFileIO.h"
#include <dirent.h>

using namespace std;

LocalIndexableMultiFileIO::LocalIndexableMultiFileIO(string fileDirectory) :
    IndexableMultiFileIO()
{  
    loadDirectories(fileDirectory);

    // make sure that there is atleast one file in the directory
    if (files.size() == 0)
        throw string("MultiFileIO: Directory is empty\n");

    // open an IO on the first file at position 0
    currentIO = new FileIO(files.at(0));
}

LocalIndexableMultiFileIO::~LocalIndexableMultiFileIO()
{
    currentIO->Close();
    delete currentIO;
}

void LocalIndexableMultiFileIO::loadDirectories(string path)
{
    DIR *dir;
    struct dirent *ent;
    dir = opendir(path.c_str());
    if (dir != NULL)
    {
        // load the stuff that's always at the begining . ..
#ifdef __win32
        readdir(dir);
        readdir(dir);
#endif

        // load only the files, don't want the folders
        while ((ent = readdir(dir)) != NULL)
        {
            string fullName(path);
            fullName += ent->d_name;
            if (opendir(fullName.c_str()) == NULL)
                files.push_back(fullName);
        }
        closedir (dir);
    }
    else
        throw string("MultiFileIO: Error opening directory\n");
}

BaseIO* LocalIndexableMultiFileIO::openFile(string path)
{
    return new FileIO(path);
}
