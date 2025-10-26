#include <XboxInternals/IO/SvodMultiFileIO.h>
#include <filesystem>

using namespace std;

SvodMultiFileIO::SvodMultiFileIO(string fileDirectory) :
    BaseIO(), addressInFile(0), fileIndex(0)
{
    loadDirectories(fileDirectory);

    // make sure that there is atleast one file in the directory
    if (files.size() == 0)
        throw string("MultiFileIO: Directory is empty\n");

    // open an IO on the first file at position 0
    currentIO = new FileIO(files.at(0));
}

SvodMultiFileIO::~SvodMultiFileIO()
{
    currentIO->Close();
    delete currentIO;
}

void SvodMultiFileIO::loadDirectories(string path)
{
    files.clear();
    
    // Modern C++20 cross-platform directory iteration
    std::filesystem::path dirPath(path);
    
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
    {
        throw string("MultiFileIO: Error opening directory\n");
    }
    
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath))
        {
            if (entry.is_regular_file())
            {
                files.push_back(entry.path().string());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        throw string("MultiFileIO: Error reading directory - ") + e.what();
    }
}

void SvodMultiFileIO::SetPosition(DWORD addressInFile, DWORD fileIndex)
{
    // check if we're in the current file
    if (fileIndex == (DWORD)-1 || fileIndex == this->fileIndex)
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
        currentIO = new FileIO(files.at(fileIndex));

        if (addressInFile >= CurrentFileLength())
            throw string("MultiFileIO: Cannot seek beyond the end of the file\n");

        this->addressInFile = addressInFile;
        this->fileIndex = fileIndex;
    }
}

void SvodMultiFileIO::GetPosition(DWORD *addressInFile, DWORD *fileIndex)
{
    *addressInFile = this->addressInFile;
    *fileIndex = this->fileIndex;
}

DWORD SvodMultiFileIO::CurrentFileLength()
{
    currentIO->SetPosition(0, ios_base::end);
    DWORD fileLen = currentIO->GetPosition();
    currentIO->SetPosition(addressInFile);

    return fileLen;
}

void SvodMultiFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
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

void SvodMultiFileIO::WriteBytes(BYTE *buffer, DWORD len)
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

void SvodMultiFileIO::Close()
{
    currentIO->Close();
}

DWORD SvodMultiFileIO::FileCount()
{
    return files.size();
}

void SvodMultiFileIO::SetPosition([[maybe_unused]] UINT64 position, [[maybe_unused]] ios_base::seekdir dir)
{
    throw string("MultiFileIO: Unused function has been called.\n");
}

UINT64 SvodMultiFileIO::GetPosition()
{
    throw string("MultiFileIO: Unused function has been called.\n");
}

void SvodMultiFileIO::Flush()
{
    currentIO->Flush();
}

UINT64 SvodMultiFileIO::Length()
{
    return currentIO->Length();
}


