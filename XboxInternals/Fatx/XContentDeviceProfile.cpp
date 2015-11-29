#include "XContentDeviceProfile.h"

XContentDeviceProfile::XContentDeviceProfile(FatxFileEntry *fileEntry, StfsPackage *profile) :
    XContentDeviceItem(fileEntry, profile)
{
}

XContentDeviceProfile::XContentDeviceProfile(std::string pathOnDevice, std::string rawName, StfsPackage *profile, DWORD fileSize) :
    XContentDeviceItem(pathOnDevice, rawName, profile, fileSize)
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

std::wstring XContentDeviceProfile::GetName()
{
    // return XContentDeviceItem::GetName();

    // check to see if the gamertag is cached
    if (gamertag.size() == 0)
    {
        ConsoleType consoleType;
        if (package != NULL)
            consoleType = package->metaData->certificate.consoleTypeFlags ? DevKit : Retail;
        else
            return L"Unknown Profile";

        try
        {
            StfsIO *accountIO = package->GetStfsIO("Account");
            Account account(accountIO, true, consoleType);

            gamertag = account.GetGamertag();
            delete accountIO;
        }
        catch (...)
        {
            return L"Unknown Profile";
        }
    }

    return gamertag;
}
