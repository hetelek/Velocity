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
    return package->metaData->thumbnailImage;
}

DWORD XContentDeviceItem::GetThumbnailSize()
{
    return package->metaData->thumbnailImageSize;
}
