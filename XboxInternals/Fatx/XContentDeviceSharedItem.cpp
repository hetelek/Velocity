#include "XContentDeviceSharedItem.h"

XContentDeviceSharedItem::XContentDeviceSharedItem(FatxFileEntry *fileEntry, IXContentHeader *package) :
    XContentDeviceItem(fileEntry, package)
{

}

XContentDeviceSharedItem::XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, IXContentHeader *package) :
    XContentDeviceItem(pathOnDevice, rawName, package)
{

}

