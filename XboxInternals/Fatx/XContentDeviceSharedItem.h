#ifndef XCONTENTDEVICESHAREDITEM_H
#define XCONTENTDEVICESHAREDITEM_H

#include "Stfs/IXContentHeader.h"
#include "XContentDeviceItem.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDeviceSharedItem : public XContentDeviceItem
{
public:
    XContentDeviceSharedItem();
    XContentDeviceSharedItem(FatxFileEntry *fileEntry, IXContentHeader *content, std::vector<std::string> contentFilePaths=std::vector<std::string>());
    XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, IXContentHeader *content, UINT64 fileSize, std::vector<std::string> contentFilePaths=std::vector<std::string>());
};

#endif // XCONTENTDEVICESHAREDITEM_H
