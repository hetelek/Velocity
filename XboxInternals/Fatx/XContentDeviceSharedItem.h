#ifndef XCONTENTDEVICESHAREDITEM_H
#define XCONTENTDEVICESHAREDITEM_H

#include "Stfs/StfsPackage.h"
#include "XContentDeviceItem.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDeviceSharedItem : public XContentDeviceItem
{
public:
    XContentDeviceSharedItem(std::string pathOnDevice, StfsPackage *package);
};

#endif // XCONTENTDEVICESHAREDITEM_H
