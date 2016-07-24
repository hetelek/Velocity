#include "BigFileIO.h"

#include <string.h>
#include <errno.h>

BigFileIO::BigFileIO(std::string filePath, bool create) :
    filePath(filePath)
{
#ifdef _WIN32
    hFile = CreateFileA(filePath.c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ,
                        NULL,
                        create ? CREATE_ALWAYS : OPEN_EXISTING,
                        0,
                        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        throw std::string("BigFileIO: Unable to open the file.");

#elif __APPLE__
    file = fopen(filePath.c_str(), "ab+");
    if (file == nullptr)
    {
        const char *errorMessage = strerror(errno);
        throw std::string("BigFileIO: Unable to open the file.");
    }

#endif
}

BigFileIO::~BigFileIO()
{
    Close();
}

void BigFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
#ifdef _WIN32
    DWORD bytesRead;
    if (!ReadFile(hFile, outBuffer, len, &bytesRead, NULL) || bytesRead != len)
        throw std::string("BigFileIO: Error reading from file.");
#elif __APPLE__
    if (fread(outBuffer, 1, len, file) != len)
        throw std::string("BigFileIO: Error reading from file.");
#endif
}

void BigFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
#ifdef _WIN32
    DWORD bytesWritten;
    if (!WriteFile(hFile, buffer, len, &bytesWritten, NULL) || bytesWritten != len)
        throw std::string("BigFileIO: Error writing to the file.");
#elif __APPLE__
    if (fwrite(buffer, 1, len, file) != len)
        throw std::string("BigFileIO: Error writing to the file.");
#endif
}

UINT64 BigFileIO::Length()
{
#ifdef _WIN32
    DWORD highFileSize;
    DWORD lowFileSize = GetFileSize(hFile, &highFileSize);

    return ((UINT64)highFileSize << 32) | lowFileSize;
#else
    UINT64 originalPosition = GetPosition();

    SetPosition(0, std::ios_base::end);
    UINT64 length = GetPosition();

    SetPosition(originalPosition, std::ios_base::beg);

    return length;
#endif
}

UINT64 BigFileIO::GetPosition()
{
#ifdef _WIN32
    LARGE_INTEGER distanceToMove = { 0, 0 };
    LARGE_INTEGER currentPosition;

    BOOL success = SetFilePointerEx(hFile, distanceToMove, &currentPosition, FILE_CURRENT);

    if (!success)
        throw std::string("BigFileIO: Error getting file position.");

    return ((UINT64)currentPosition.u.HighPart << 32) | currentPosition.u.LowPart;
#elif __APPLE__
    off_t address;
    if (fgetpos(file, &address) != 0)
        throw std::string("BigFileIO: Error getting file position.");

    return static_cast<UINT64>(address);
#endif
}

void BigFileIO::SetPosition(UINT64 position, std::ios_base::seek_dir dir)
{
#ifdef _WIN32
    DWORD moveMethod = FILE_BEGIN;
    switch (dir)
    {
        case std::ios_base::cur:
            moveMethod = FILE_CURRENT;
            break;
        case std::ios_base::end:
            moveMethod = FILE_END;
            break;
    }

    // high and low 32 bits must be separated for the windows function call
    DWORD posHigh = (DWORD)(position >> 32);
    DWORD posLow = position & 0xFFFFFFFF;

    DWORD posHighWritten = posHigh;
    DWORD posLowWritten = SetFilePointer(hFile, posLow, &posHighWritten, moveMethod);

    if (posLowWritten != posLow || posHighWritten != posHigh)
        throw std::string("BigFileIO: Error setting position.");
#elif __APPLE__
    UINT64 address = position;
    switch (dir)
    {
    case std::ios_base::cur:
        address += this->GetPosition();
        break;
    case std::ios_base::end:
        address += this->Length();
        break;
    }

    if (fseek(file, address, SEEK_SET) != 0)
    {
        perror("yoyo");
        throw std::string("BigFileIO: Error setting position.");
    }
#endif
}

void BigFileIO::Close()
{
#ifdef _WIN32
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
#elif __APPLE__
    fclose(file);
    file = nullptr;
#endif
}

void BigFileIO::Flush()
{
#ifdef _WIN32
    FlushFileBuffers(hFile);
#elif __APPLE__
    fflush(file);
#endif
}
