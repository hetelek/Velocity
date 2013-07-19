#include "XContentDeviceSharedItem.h"

XContentDeviceSharedItem::XContentDeviceSharedItem(FatxFileEntry *fileEntry, StfsPackage *package) :
    XContentDeviceItem(fileEntry, package)
{

}

XContentDeviceSharedItem::XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, StfsPackage *package) :
    XContentDeviceItem(pathOnDevice, rawName, package)
{

}

