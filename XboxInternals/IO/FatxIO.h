#ifndef FATXIO_H
#define FATXIO_H

#include "DeviceIO.h"
#include "../Fatx/FatxConstants.h"

class XBOXINTERNALSSHARED_EXPORT FatxIO : public BaseIO
{
public:
    FatxIO(DeviceIO *device, FatxFileEntry *entry);
    FatxFileEntry GetFatxFileEntry();

    void ReadBytes(BYTE *outBuffer, DWORD len);
    void WriteBytes(BYTE *buffer, DWORD len);
    void SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg);
    UINT64 GetPosition();

    void Close();
    void Flush();

private:
    DeviceIO *device;
    FatxFileEntry *entry;
    DWORD pos;
    DWORD maxReadConsecutive;
};

#endif // FATXIO_H
