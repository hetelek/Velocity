#include "XContentDeviceSharedItem.h"

XContentDeviceSharedItem::XContentDeviceSharedItem(FatxFileEntry *fileEntry, IXContentHeader *package, std::vector<std::string> contentFilePaths) :
    XContentDeviceItem(fileEntry, package, contentFilePaths)
{

}

XContentDeviceSharedItem::XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, IXContentHeader *content, UINT64 fileSize, std::vector<std::string> contentFilePaths) :
    XContentDeviceItem(pathOnDevice, rawName, content, fileSize, contentFilePaths)
{

}

