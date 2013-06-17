#ifndef XCONTENTDEVICEPROFILE_H
#define XCONTENTDEVICEPROFILE_H

#include <iostream>
#include <vector>

#include "Stfs/StfsPackage.h"
#include "XContentDeviceItem.h"
#include "XContentDeviceTitle.h"

class XContentDeviceProfile : public XContentDeviceItem
{
public:
    XContentDeviceProfile(std::string pathOnDevice, StfsPackage *profile);

    std::vector<XContentDeviceTitle> titles;
};

#endif // XCONTENTDEVICEPROFILE_H
