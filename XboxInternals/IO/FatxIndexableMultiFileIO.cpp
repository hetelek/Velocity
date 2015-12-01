#include "FatxIndexableMultiFileIO.h"

FatxIndexableMultiFileIO::FatxIndexableMultiFileIO(std::string fileDirectory, FatxDrive *drive) :
    IndexableMultiFileIO(), drive(drive)
{
    loadDirectories(fileDirectory);

    // make sure that there is atleast one file in the directory
    if (files.size() == 0)
        throw string("MultiFileIO: Directory is empty\n");

    // open an IO on the first file at position 0
    currentIO = openFile(files.at(0));
}

FatxIndexableMultiFileIO::~FatxIndexableMultiFileIO()
{
    currentIO->Close();
    delete currentIO;
}


void FatxIndexableMultiFileIO::loadDirectories(std::string path)
{
    FatxFileEntry *directory = drive->GetFileEntry(path);
    drive->GetChildFileEntries(directory);

    // get the paths of all the files in the directory
    for (size_t i = 0; i < directory->cachedFiles.size(); i++)
    {
        // skip over deleted files
        FatxFileEntry directoryFile = directory->cachedFiles.at(i);
        if (directoryFile.nameLen == FATX_ENTRY_DELETED)
            continue;

        // get the path of the file
        std::string filePath = directoryFile.path + directoryFile.name;
        files.push_back(filePath);
    }
}

BaseIO *FatxIndexableMultiFileIO::openFile(std::string path)
{
    FatxFileEntry *entry = drive->GetFileEntry(path);
    return new FatxIO(drive->GetFatxIO(entry));
}
