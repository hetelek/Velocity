#include "MultiFileIO.h"

using namespace std;

MultiFileIO::MultiFileIO(string fileDirectory) :
    BaseIO(), fileIndex(0), addressInFile(0)
{  
    loadDirectories(fileDirectory);

    // make sure that there is atleast one file in the directory
    if (files.size() == 0)
        throw string("MultiFileIO: Directory is empty\n");

    // open an IO on the first file at position 0
    currentIO = new FileIO(files.at(0));
}

MultiFileIO::~MultiFileIO()
{
    currentIO->close();
    delete currentIO;
}

void MultiFileIO::loadDirectories(string path)
{
    DIR *dir;
    struct dirent *ent;
    dir = opendir(path.c_str());
    if (dir != NULL)
    {
        // load the stuff that's always at the begining . ..
        readdir(dir);
        readdir(dir);

        // load only the files, don't want the folders
        while ((ent = readdir(dir)) != NULL)
        {
            string fullName = path + string(ent->d_name);
            if (opendir(fullName.c_str()) == NULL)
                files.push_back(fullName);
        }
        closedir (dir);
    }
    else
        throw string("MultiFileIO: Error opening directory\n");
}

void MultiFileIO::SetPosition(DWORD addressInFile, int fileIndex)
{
    // check if we're in the current file
    if (fileIndex == -1 || fileIndex == this->fileIndex)
    {
        if (addressInFile >= CurrentFileLength())
            throw string("MultiFileIO: Cannot seek beyond the end of the file\n");

        currentIO->setPosition(addressInFile);

        this->addressInFile = addressInFile;
    }
    else
    {
        if (fileIndex >= files.size())
            throw string("MultiFileIO: Specified file index is out of range\n");

        // open a new IO on the file
        currentIO->close();
        delete currentIO;
        currentIO = new FileIO(files.at(fileIndex));

        if (addressInFile >= CurrentFileLength())
            throw string("MultiFileIO: Cannot seek beyond the end of the file\n");

        this->addressInFile = addressInFile;
        this->fileIndex = fileIndex;
    }
}

void MultiFileIO::GetPosition(DWORD *addressInFile, int *fileIndex)
{
    *addressInFile = this->addressInFile;
    *fileIndex = this->fileIndex;
}

DWORD MultiFileIO::CurrentFileLength()
{
    currentIO->setPosition(0, ios_base::end);
    DWORD fileLen = currentIO->getPosition();
    currentIO->setPosition(addressInFile);

    return fileLen;
}

void MultiFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    while (len)
    {
        // calculate bytes to read in current file
        DWORD bytesLeft = CurrentFileLength() - addressInFile;
        DWORD amountToRead = (bytesLeft > len) ? len : bytesLeft;

        currentIO->readBytes(outBuffer, amountToRead);

        // seek to the next file if there's more to read
        if (len >= bytesLeft && (fileIndex + 1) < FileCount())
            SetPosition(0, fileIndex + 1);
        else if (len < bytesLeft)
            SetPosition(this->addressInFile + len);

        // update values for next iteration
        len -= amountToRead;
        outBuffer += amountToRead;
    }
}

void MultiFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
    while (len)
    {
        // calculate bytes to read in current file
        DWORD bytesLeft = CurrentFileLength() - addressInFile;
        DWORD amountToRead = (bytesLeft > len) ? len : bytesLeft;

        currentIO->write(buffer, amountToRead);

        // seek to the next file if there's more to read
        if (len >= bytesLeft)
            SetPosition(0, fileIndex + 1);
        else
            SetPosition(this->addressInFile + len);

        // update values for next iteration
        len -= amountToRead;
        buffer += amountToRead;
    }
}

void MultiFileIO::Close()
{
    currentIO->close();
}

DWORD MultiFileIO::FileCount()
{
    return files.size();
}
