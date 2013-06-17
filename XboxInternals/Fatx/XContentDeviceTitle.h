#ifndef XCONTENTDEVICETITLE_H
#define XCONTENTDEVICETITLE_H

#include <iostream>
#include "Stfs/StfsPackage.h"
#include "XContentDeviceItem.h"

class XContentDeviceTitle : public XContentDeviceItem
{
public:
    XContentDeviceTitle(std::string pathOnDevice);

    std::wstring GetName();
    BYTE* GetThumbnail();
    DWORD GetThumbnailSize();
    DWORD GetTitleID();

    vector<XContentDeviceItem> titleSaves;

private:
    std::string pathOnDevice;
};

#endif // XCONTENTDEVICETITLE_H
