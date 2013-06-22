#include "XContentDeviceProfile.h"

XContentDeviceProfile::XContentDeviceProfile(std::string pathOnDevice, std::string rawName, StfsPackage *profile) :
    XContentDeviceItem(pathOnDevice, rawName, profile)
{
}

BYTE *XContentDeviceProfile::GetProfileID()
{
    if (package != NULL)
    {
        return package->metaData->profileID;
    }
    else
    {
        if (titles.size() != 0 && titles.at(0).titleSaves.size() != 0)
            return titles.at(0).titleSaves.at(0).package->metaData->profileID;
        else
            return NULL;
    }
}
