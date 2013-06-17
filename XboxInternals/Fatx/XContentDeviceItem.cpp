#include "XContentDeviceItem.h"

XContentDeviceItem::XContentDeviceItem(std::string pathOnDevice, StfsPackage *package) :
    package(package), pathOnDevice(pathOnDevice)
{

}

std::string XContentDeviceItem::GetPathOnDevice()
{
    return pathOnDevice;
}

std::wstring XContentDeviceItem::GetName()
{
    if (package == NULL)
        return L"";

    if (package->metaData->contentType == Profile)
    {
        // TODO: make this retrieve the gamertag
        return package->metaData->displayName;
    }
    else
    {
        return package->metaData->displayName;
    }
}

BYTE *XContentDeviceItem::GetThumbnail()
{
    if (package == NULL)
        return NULL;
    return package->metaData->thumbnailImage;
}

DWORD XContentDeviceItem::GetThumbnailSize()
{
    if (package == NULL)
        return NULL;
    return package->metaData->thumbnailImageSize;
}
