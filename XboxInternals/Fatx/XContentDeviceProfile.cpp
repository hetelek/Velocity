#include "XContentDeviceProfile.h"

XContentDeviceProfile::XContentDeviceProfile() :
    XContentDeviceItem()
{

}

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
    if (content != NULL)
    {
        return content->metaData->profileID;
    }
    else
    {
        if (titles.size() != 0 && titles.at(0).titleSaves.size() != 0)
            return titles.at(0).titleSaves.at(0).content->metaData->profileID;
        else
            return NULL;
    }
}

std::wstring XContentDeviceProfile::GetName()
{
    // check to see if the gamertag is cached
    if (gamertag.size() == 0)
    {
        ConsoleType consoleType;
        if (content != NULL)
            consoleType = content->metaData->certificate.consoleTypeFlags ? DevKit : Retail;
        else
            return L"Unknown Profile";

        try
        {
            StfsPackage *stfsPackage = reinterpret_cast<StfsPackage*>(content);

            StfsIO *accountIO = stfsPackage->GetStfsIO("Account");
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
