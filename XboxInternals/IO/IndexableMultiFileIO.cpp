#include "IndexableMultiFileIO.h"
#include <dirent.h>

using namespace std;

IndexableMultiFileIO::IndexableMultiFileIO() :
    addressInFile(0), fileIndex(0)
{

}

IndexableMultiFileIO::~IndexableMultiFileIO()
{
}

void IndexableMultiFileIO::SetPosition(DWORD addressInFile, int fileIndex)
{
    // check if we're in the current file
    if (fileIndex == -1 || fileIndex == this->fileIndex)
    {
        if (addressInFile >= CurrentFileLength())
            throw string("MultiFileIO: Cannot seek beyond the end of the file\n");

        currentIO->SetPosition(addressInFile);

        this->addressInFile = addressInFile;
    }
    else
    {
        if (fileIndex >= files.size())
            throw string("MultiFileIO: Specified file index is out of range\n");

        // open a new IO on the file
        currentIO->Close();
        delete currentIO;
        currentIO = openFile(files.at(fileIndex));

        if (addressInFile >= CurrentFileLength())
            throw string("MultiFileIO: Cannot seek beyond the end of the file\n");

        this->addressInFile = addressInFile;
        this->fileIndex = fileIndex;
    }
}

void IndexableMultiFileIO::GetPosition(DWORD *addressInFile, DWORD *fileIndex)
{
    *addressInFile = this->addressInFile;
    *fileIndex = this->fileIndex;
}

DWORD IndexableMultiFileIO::CurrentFileLength()
{
    currentIO->SetPosition(0, ios_base::end);
    DWORD fileLen = currentIO->GetPosition();
    currentIO->SetPosition(addressInFile);

    return fileLen;
}

void IndexableMultiFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    while (len)
    {
        // calculate bytes to read in current file
        DWORD bytesLeft = CurrentFileLength() - addressInFile;
        DWORD amountToRead = (bytesLeft > len) ? len : bytesLeft;

        currentIO->ReadBytes(outBuffer, amountToRead);

        // seek to the next file if there's more to read
        if (len >= bytesLeft && (fileIndex + 1) < FileCount())
            SetPosition((DWORD)0, fileIndex + 1);
        else if (len < bytesLeft)
            SetPosition((DWORD)(this->addressInFile + len));

        // update values for next iteration
        len -= amountToRead;
        outBuffer += amountToRead;
    }
}

void IndexableMultiFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
    while (len)
    {
        // calculate bytes to read in current file
        DWORD bytesLeft = CurrentFileLength() - addressInFile;
        DWORD amountToRead = (bytesLeft > len) ? len : bytesLeft;

        currentIO->Write(buffer, amountToRead);

        // seek to the next file if there's more to read
        if (len >= bytesLeft)
            SetPosition((DWORD)0, fileIndex + 1);
        else
            SetPosition((DWORD)(this->addressInFile + len));

        // update values for next iteration
        len -= amountToRead;
        buffer += amountToRead;
    }
}

void IndexableMultiFileIO::Close()
{
    currentIO->Close();
}

DWORD IndexableMultiFileIO::FileCount()
{
    return files.size();
}

void IndexableMultiFileIO::SetPosition(UINT64 position, ios_base::seek_dir dir)
{
    throw string("MultiFileIO: Unused function has been called.\n");
}

UINT64 IndexableMultiFileIO::GetPosition()
{
    throw string("MultiFileIO: Unused function has been called.\n");
}

void IndexableMultiFileIO::Flush()
{
    currentIO->Flush();
}

UINT64 IndexableMultiFileIO::Length()
{
    return currentIO->Length();
}
