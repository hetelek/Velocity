#ifndef XCONTENTDEVICE_H
#define XCONTENTDEVICE_H

#include <iostream>
#include <vector>
#include <ctype.h>
#include <iomanip>

#ifndef __WIN32
    #include <libgen.h>
    #include <sys/stat.h>
#else
    #include <Shlwapi.h>
#endif

#include "Stfs/StfsPackage.h"
#include "FatxDrive.h"
#include "IO/FatxIO.h"
#include "Disc/Svod.h"
#include "XContentDeviceProfile.h"
#include "XContentDeviceSharedItem.h"
#include "Utils.h"

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
    std::vector<XContentDeviceSharedItem> *updates;
    std::vector<XContentDeviceSharedItem> *systemItems;

    bool LoadDevice(void(*progress)(void*, bool) = NULL, void *arg = NULL);
    FatxDriveType GetDeviceType();
    UINT64 GetFreeMemory(void(*progress)(void*, bool) = NULL, void *arg = NULL, bool finish = true);
    UINT64 GetTotalMemory();
    std::wstring GetName();
    void SetName(std::wstring name);
    FatxDrive *GetFatxDrive();

    void CopyFileToLocalDisk(std::string outPath, std::string inPath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);
    void CopyFileToDevice(std::string outPath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);
    void CopyFileToRawDevice(std::string outPath, std::string name, std::string inPath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);
    void DeleteFile(IXContentHeader *package, std::string pathOnDevice);

private:
    FatxDrive *drive;
    Partition *content;

    bool ValidOfflineXuid(std::string xuid);
    bool ValidTitleID(std::string id);
    void GetAllContentItems(FatxFileEntry &titleFolder, vector<XContentDeviceItem> &itemsFound, void(*progress)(void*, bool) = NULL, void *arg = NULL);
    void CleanupSharedFiles(std::vector<XContentDeviceSharedItem> *category);
    std::string ToUpper(std::string str);
};

#endif // XCONTENTDEVICE_H
