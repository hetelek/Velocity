#include "XContentDeviceItem.h"

XContentDeviceItem::XContentDeviceItem(std::string pathOnDevice, std::string rawName, StfsPackage *package) :
    package(package), pathOnDevice(pathOnDevice), rawName(rawName)
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

std::string XContentDeviceItem::GetRawName()
{
    return rawName;
}
