#include "BigFileIO.h"

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
#endif
}

void BigFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
#ifdef _WIN32
    DWORD bytesWritten;
    if (!WriteFile(hFile, buffer, len, &bytesWritten, NULL) || bytesWritten != len)
        throw std::string("BigFileIO: Error writing to the file.");
#endif
}

UINT64 BigFileIO::Length()
{
#ifdef _WIN32
    DWORD highFileSize;
    DWORD lowFileSize = GetFileSize(hFile, &highFileSize);

    return ((UINT64)highFileSize << 32) | lowFileSize;
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
#endif
}

void BigFileIO::Close()
{
#ifdef _WIN32
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
#endif
}

void BigFileIO::Flush()
{
#ifdef _WIN32
    FlushFileBuffers(hFile);
#endif
}
