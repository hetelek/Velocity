#include "XContentDeviceItem.h"

XContentDeviceItem::XContentDeviceItem(FatxFileEntry *fileEntry, StfsPackage *package) :
    package(package), pathOnDevice(fileEntry->path + fileEntry->name), rawName(fileEntry->name), fileSize(fileEntry->fileSize)
{

}

XContentDeviceItem::XContentDeviceItem(std::string pathOnDevice, std::string rawName, StfsPackage *package, DWORD fileSize) :
    package(package), pathOnDevice(pathOnDevice), rawName(rawName), fileSize(fileSize)
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
        return 0;
    return package->metaData->thumbnailImageSize;
}

std::string XContentDeviceItem::GetRawName()
{
    return rawName;
}

BYTE *XContentDeviceItem::GetProfileID()
{
    if (package == NULL)
        return NULL;
    return package->metaData->profileID;
}

DWORD XContentDeviceItem::GetFileSize()
{
    return fileSize;
}
