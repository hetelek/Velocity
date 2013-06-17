#include "XContentDeviceTitle.h"

XContentDeviceTitle::XContentDeviceTitle(std::string pathOnDevice) :
    pathOnDevice(pathOnDevice), XContentDeviceItem(pathOnDevice, NULL)
{

}

std::wstring XContentDeviceTitle::GetName()
{
    if (titleSaves.size() == 0)
        return L"";
    return titleSaves.at(0).package->metaData->titleName;
}

BYTE *XContentDeviceTitle::GetThumbnail()
{
    if (titleSaves.size() == 0)
        return NULL;
    return titleSaves.at(0).package->metaData->thumbnailImage;
}

DWORD XContentDeviceTitle::GetThumbnailSize()
{
    if (titleSaves.size() == 0)
        return 0;
    return titleSaves.at(0).package->metaData->thumbnailImageSize;
}

DWORD XContentDeviceTitle::GetTitleID()
{
    if (titleSaves.size() == 0)
        return 0;
    return titleSaves.at(0).package->metaData->titleID;
}
