#include "XContentDevice.h"

XContentDevice::XContentDevice(FatxDrive *drive) :
    drive(drive)
{

}

XContentDevice::~XContentDevice()
{
    for (int i = 0; i < profiles.size(); i++)
    {
        delete profiles.at(i).package;
        for (int x = 0; x < profiles.at(i).titles.size(); x++)
            for (int y = 0; y < profiles.at(i).titles.at(x).titleSaves.size(); y++)
                delete profiles.at(i).titles.at(x).titleSaves.at(y).package;
    }
}

bool XContentDevice::LoadDevice()
{
    // the generic path for content is:
    // Drive:\Content\Content\OFFLINE_XUID\TITLE_ID\CONTENT_TYPE\STFS_PACKAGE

    FatxFileEntry *fileEntry = drive->GetFileEntry("Drive:\\Content\\Content\\");
    if (fileEntry == NULL)
        return false;

    // load all of the sub dirents in Drive:\Content\Content
    drive->GetChildFileEntries(fileEntry);

    // check for profile folders
    for (int i = 0; i< fileEntry->cachedFiles.size(); i++)
    {
        FatxFileEntry profileFolderEntry = fileEntry->cachedFiles.at(i);

        // all profile folders are named the profile's offline XUID
        if ((profileFolderEntry.fileAttributes & FatxDirectory) == 0 || !ValidOfflineXuid(profileFolderEntry.name))
            continue;

        // check for a profile file
        std::string profilePath = "Drive:\\Content\\Content\\" + profileFolderEntry.name + "\\FFFE07D1\\00010000\\";
        FatxFileEntry* profileEntry = drive->GetFileEntry(profilePath + profileFolderEntry.name);

        StfsPackage *profilePackage = NULL;
        if (profileEntry != NULL)
        {
            FatxIO io = drive->GetFatxIO(profileEntry);
            profilePackage = new StfsPackage(new FatxIO(io), StfsPackageDeleteIO);
        }
        else
        {
            profilePath = "";
        }
        XContentDeviceProfile profile(profilePath, profilePackage);

        // get all of the game save data for this profile
        drive->GetChildFileEntries(&profileFolderEntry);
        for (int x = 0; x < profileFolderEntry.cachedFiles.size(); x++)
        {
            // verify that the entry is a valid title folder, should be named with title ID
            FatxFileEntry titleFolder = profileFolderEntry.cachedFiles.at(x);
            if ((titleFolder.fileAttributes & FatxDirectory) == 0 || !ValidTitleID(titleFolder.name) || titleFolder.name == "FFFE07D1")
                continue;

            XContentDeviceTitle title(titleFolder.path + "\\" + titleFolder.name);

            // load all of the sub folders
            drive->GetChildFileEntries(&titleFolder);

            // iterate through all of the content types for this title
            for (int y = 0; y < titleFolder.cachedFiles.size(); y++)
            {
                // verify that the entry is a folder and named with the content type as a string in hex,
                // so for savegames the folder would be named 00000001
                FatxFileEntry contentTypeFolder = titleFolder.cachedFiles.at(y);
                if ((contentTypeFolder.fileAttributes & FatxDirectory) == 0 || !ValidTitleID(contentTypeFolder.name))
                    continue;

                // load all of the content items in this directory
                drive->GetChildFileEntries(&contentTypeFolder);

                // iterate through all of the STFS packages in this content type folder
                for (int z = 0; z < contentTypeFolder.cachedFiles.size(); z++)
                {
                    // we're looking for STFS packages, so make sure the entry isn't another directory
                    FatxFileEntry contentPackage = contentTypeFolder.cachedFiles.at(z);
                    if (contentPackage.fileAttributes & FatxDirectory)
                        continue;

                    // open an IO on this file, should be STFS package
                    FatxIO io = drive->GetFatxIO(&contentPackage);
                    DWORD fileMagic = io.ReadDword();

                    // verify the magic
                    if (fileMagic != CON && fileMagic != LIVE && fileMagic != PIRS)
                        continue;

                    // this might not be a valid STFS package, so we have to do try {} catch {}
                    StfsPackage *content;
                    try
                    {
                        content = new StfsPackage(new FatxIO(io), StfsPackageDeleteIO);
                    }
                    catch (...)
                    {
                        continue;
                    }

                    XContentDeviceItem item(contentPackage.path, content);

                    title.titleSaves.push_back(item);
                }
            }

            if (title.titleSaves.size() != 0)
                profile.titles.push_back(title);
        }


        profiles.push_back(profile);
    }
}

bool XContentDevice::ValidOfflineXuid(std::string xuid)
{
    // offline XUIDs must be 16 hex characters long and start with E
    // ex. E838DF04A2A46AEF

    if (xuid.size() != 16 || xuid.at(0) != 'E')
        return false;

    for (int i = 1; i < 16; i++)
        if (!isxdigit(xuid.at(i)))
            return false;

    return true;
}

bool XContentDevice::ValidTitleID(std::string id)
{
    // all title id folders and content type folders are 8 hex digits long

    if (id.size() != 8)
        return false;

    for (int i = 0; i < 8; i++)
        if (!isxdigit(id.at(i)))
            return false;

    return true;
}
