#include "XContentDeviceItem.h"

XContentDeviceItem::XContentDeviceItem() :
    content(NULL)
{

}

XContentDeviceItem::XContentDeviceItem(FatxFileEntry *fileEntry, IXContentHeader *content, std::vector<std::string> contentFilePaths) :
    content(content), pathOnDevice(fileEntry->path + fileEntry->name), rawName(fileEntry->name), fileSize(fileEntry->fileSize), contentFilePaths(contentFilePaths)
{

}

XContentDeviceItem::XContentDeviceItem(std::string pathOnDevice, std::string rawName, IXContentHeader *content, UINT64 fileSize, std::vector<std::string> contentFilePaths) :
    content(content), pathOnDevice(pathOnDevice), rawName(rawName), fileSize(fileSize), contentFilePaths(contentFilePaths)
{

}

std::string XContentDeviceItem::GetPathOnDevice()
{
    return pathOnDevice;
}

std::wstring XContentDeviceItem::GetName()
{
    if (content == NULL)
        return L"";

    return content->metaData->displayName;
}

BYTE *XContentDeviceItem::GetThumbnail()
{
    if (content == NULL)
        return NULL;
    return content->metaData->thumbnailImage;
}

DWORD XContentDeviceItem::GetThumbnailSize()
{
    if (content == NULL)
        return 0;
    return content->metaData->thumbnailImageSize;
}

std::string XContentDeviceItem::GetRawName()
{
    return rawName;
}

BYTE *XContentDeviceItem::GetProfileID()
{
    if (content == NULL)
        return NULL;
    return content->metaData->profileID;
}

UINT64 XContentDeviceItem::GetFileSize()
{
    UINT64 toReturn = fileSize;
    if (content != NULL)
        toReturn += content->metaData->dataFileCombinedSize;
    return toReturn;
}

std::vector<std::string> XContentDeviceItem::GetContentFilePaths()
{
    return contentFilePaths;
}
