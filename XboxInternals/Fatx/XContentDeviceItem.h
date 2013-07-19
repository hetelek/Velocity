#ifndef XCONTENTDEVICEITEM_H
#define XCONTENTDEVICEITEM_H

#include <iostream>
#include "Stfs/StfsPackage.h"
#include "Fatx/FatxDrive.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDeviceItem
{
public:
    XContentDeviceItem(FatxFileEntry *fileEntry, StfsPackage *package);
    XContentDeviceItem(std::string pathOnDevice, std::string rawName, StfsPackage *package, DWORD fileSize = 0);

    StfsPackage *package;

    virtual std::string GetPathOnDevice();
    virtual std::wstring GetName();
    virtual BYTE* GetThumbnail();
    virtual DWORD GetThumbnailSize();
    virtual std::string GetRawName();
    virtual BYTE* GetProfileID();
    virtual DWORD GetFileSize();

private:
    std::string pathOnDevice;
    std::string rawName;
    DWORD fileSize;
};

#endif // XCONTENTDEVICEITEM_H
