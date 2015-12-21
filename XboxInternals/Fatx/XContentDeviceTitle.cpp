#include "XContentDeviceTitle.h"

XContentDeviceTitle::XContentDeviceTitle() :
    XContentDeviceItem()
{

}

XContentDeviceTitle::XContentDeviceTitle(std::string pathOnDevice, std::string rawName) :
    pathOnDevice(pathOnDevice), XContentDeviceItem(pathOnDevice, rawName, NULL)
{

}

std::wstring XContentDeviceTitle::GetName()
{
    if (titleSaves.size() == 0)
        return L"";
    return titleSaves.at(0).content->metaData->titleName;
}

BYTE *XContentDeviceTitle::GetThumbnail()
{
    if (titleSaves.size() == 0)
        return NULL;
    else if (titleSaves.at(0).content->metaData->titleThumbnailImage != NULL)
       return titleSaves.at(0).content->metaData->titleThumbnailImage;
    else
        return titleSaves.at(0).content->metaData->thumbnailImage;
}

DWORD XContentDeviceTitle::GetThumbnailSize()
{
    if (titleSaves.size() == 0)
        return 0;
    return titleSaves.at(0).content->metaData->titleThumbnailImageSize;
}

DWORD XContentDeviceTitle::GetTitleID()
{
    if (titleSaves.size() == 0)
        return 0;
    return titleSaves.at(0).content->metaData->titleID;
}

BYTE *XContentDeviceTitle::GetProfileID()
{
    if (titleSaves.size() == 0)
        return NULL;
    return titleSaves.at(0).content->metaData->profileID;
}
