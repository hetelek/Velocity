#ifndef XCONTENTDEVICE_H
#define XCONTENTDEVICE_H

#include <iostream>
#include <vector>
#include <ctype.h>

#include "Stfs/StfsPackage.h"
#include "FatxDrive.h"
#include "IO/FatxIO.h"
#include "XContentDeviceProfile.h"

class XContentDevice
{
public:
    XContentDevice(FatxDrive *drive);
    ~XContentDevice();

    bool LoadDevice();

    std::vector<XContentDeviceProfile> profiles;

private:
    FatxDrive *drive;

    bool ValidOfflineXuid(std::string xuid);

    bool ValidTitleID(std::string id);
};

#endif // XCONTENTDEVICE_H
