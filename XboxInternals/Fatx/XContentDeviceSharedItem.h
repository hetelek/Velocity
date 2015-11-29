#ifndef XCONTENTDEVICESHAREDITEM_H
#define XCONTENTDEVICESHAREDITEM_H

#include "Stfs/IXContentHeader.h"
#include "XContentDeviceItem.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDeviceSharedItem : public XContentDeviceItem
{
public:
    XContentDeviceSharedItem(FatxFileEntry *fileEntry, IXContentHeader *content);
    XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, IXContentHeader *content);
};

#endif // XCONTENTDEVICESHAREDITEM_H
