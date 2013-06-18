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

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDevice
{
public:
    XContentDevice(FatxDrive *drive);
    ~XContentDevice();

    std::vector<XContentDeviceProfile> *profiles;

    std::vector<XContentDeviceSharedItem> *games;
    std::vector<XContentDeviceSharedItem> *dlc;
    std::vector<XContentDeviceSharedItem> *demos;
    std::vector<XContentDeviceSharedItem> *videos;
    std::vector<XContentDeviceSharedItem> *themes;
    std::vector<XContentDeviceSharedItem> *gamerPictures;
    std::vector<XContentDeviceSharedItem> *avatarItems;
    std::vector<XContentDeviceSharedItem> *systemItems;

    bool LoadDevice(void(*progress)(void*, bool) = NULL, void *arg = NULL);
    FatxDriveType GetDeviceType();
    UINT64 GetFreeMemory(void(*progress)(void*, bool) = NULL, void *arg = NULL);
    UINT64 GetTotalMemory();
    std::wstring GetName();


private:
    FatxDrive *drive;
    Partition *content;

    bool ValidOfflineXuid(std::string xuid);
    bool ValidTitleID(std::string id);
    void GetAllContentItems(FatxFileEntry &titleFolder, vector<XContentDeviceItem> &itemsFound);
    void CleanupSharedFiles(std::vector<XContentDeviceSharedItem> *category);
};

#endif // XCONTENTDEVICE_H
