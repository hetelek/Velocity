#ifndef XCONTENTDEVICE_H
#define XCONTENTDEVICE_H

#include <iostream>
#include <vector>
#include <ctype.h>

#include "Stfs/StfsPackage.h"
#include "FatxDrive.h"
#include "IO/FatxIO.h"
#include "XContentDeviceProfile.h"
#include "XContentDeviceSharedItem.h"

class XContentDevice
{
public:
    XContentDevice(FatxDrive *drive);
    ~XContentDevice();

    bool LoadDevice();

    std::vector<XContentDeviceProfile> *profiles;

    std::vector<XContentDeviceSharedItem> *demos;
    std::vector<XContentDeviceSharedItem> *videos;
    std::vector<XContentDeviceSharedItem> *themes;
    std::vector<XContentDeviceSharedItem> *gamerPictures;
    std::vector<XContentDeviceSharedItem> *avatarItems;
    std::vector<XContentDeviceSharedItem> *systemItems;
    std::vector<XContentDeviceSharedItem> *music;


private:
    FatxDrive *drive;

    bool ValidOfflineXuid(std::string xuid);

    bool ValidTitleID(std::string id);

    void GetAllContentItems(FatxFileEntry &titleFolder, vector<XContentDeviceItem> &itemsFound);
};

#endif // XCONTENTDEVICE_H
