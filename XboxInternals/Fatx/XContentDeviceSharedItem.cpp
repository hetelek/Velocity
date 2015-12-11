#include "XContentDeviceSharedItem.h"

XContentDeviceSharedItem::XContentDeviceSharedItem(FatxFileEntry *fileEntry, IXContentHeader *package, std::vector<std::string> contentFilePaths) :
    XContentDeviceItem(fileEntry, package, contentFilePaths)
{

}

XContentDeviceSharedItem::XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, IXContentHeader *package, std::vector<std::string> contentFilePaths) :
    XContentDeviceItem(pathOnDevice, rawName, package, 0, contentFilePaths)
{

}

