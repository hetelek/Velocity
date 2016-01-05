#ifndef BIGFILEIO_H
#define BIGFILEIO_H

#ifdef _WIN32
    #include <windows.h>
#endif

#include <string>

#include "BaseIO.h"

// FileIO to handle files larger then 4GB
class BigFileIO : public BaseIO
{
public:
    BigFileIO(std::string filePath, bool create = false);
    ~BigFileIO();

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    UINT64 Length();

    UINT64 GetPosition();

    void SetPosition(UINT64 position, std::ios_base::seek_dir dir);

    void Close();

    void Flush();

private:
    std::string filePath;

#ifdef _WIN32
    HANDLE hFile;
#endif
};

#endif // BIGFILEIO_H
