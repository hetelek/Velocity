#ifndef XCONTENTDEVICEITEM_H
#define XCONTENTDEVICEITEM_H

#include <iostream>
#include "Stfs/IXContentHeader.h"
#include "Fatx/FatxDrive.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDeviceItem
{
public:
    XContentDeviceItem();
    XContentDeviceItem(FatxFileEntry *fileEntry, IXContentHeader *content, std::vector<std::string> contentFilePaths=std::vector<std::string>());
    XContentDeviceItem(std::string pathOnDevice, std::string rawName, IXContentHeader *content, UINT64 fileSize = 0, std::vector<std::string> contentFilePaths=std::vector<std::string>());

    IXContentHeader *content;

    virtual std::string GetPathOnDevice();
    virtual std::wstring GetName();
    virtual BYTE* GetThumbnail();
    virtual DWORD GetThumbnailSize();
    virtual std::string GetRawName();
    virtual BYTE* GetProfileID();
    virtual UINT64 GetFileSize();

    std::vector<std::string> GetContentFilePaths();

private:
    std::string pathOnDevice;
    std::vector<std::string> contentFilePaths;
    std::string rawName;
    UINT64 fileSize;
};

#endif // XCONTENTDEVICEITEM_H
