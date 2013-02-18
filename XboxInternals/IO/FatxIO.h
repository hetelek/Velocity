#ifndef FATXIO_H
#define FATXIO_H

#include "DeviceIO.h"
#include "FileIO.h"
#include "../Fatx/FatxConstants.h"

#include <iostream>
#include <vector>

struct Range
{
    UINT64 start;
    UINT64 len;
};

class XBOXINTERNALSSHARED_EXPORT FatxIO : public BaseIO
{
public:
    FatxIO(DeviceIO *device, FatxFileEntry *entry);

    FatxFileEntry GetFatxFileEntry();

    void SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg);

    void Flush();

    void Close();

    UINT64 GetPosition();

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    void SaveFile(std::string savePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

private:
    DeviceIO *device;
    FatxFileEntry *entry;
    UINT64 pos;
    DWORD maxReadConsecutive;
};

#endif // FATXIO_H
