#include "LocalIndexableMultiFileIO.h"

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
    files = Utils::FilesInDirectory(path);

    // sort the files so they're in order (Data0000, Data0001, etc)
    std::sort(files.begin(), files.end());
}

BaseIO* LocalIndexableMultiFileIO::openFile(string path)
{
    return new FileIO(path);
}
