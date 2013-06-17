#include "XContentDevice.h"

XContentDevice::XContentDevice(FatxDrive *drive) :
    drive(drive)
{
    profiles = new std::vector<XContentDeviceProfile>();

    games = new std::vector<XContentDeviceSharedItem>();
    dlc = new std::vector<XContentDeviceSharedItem>();
    demos = new std::vector<XContentDeviceSharedItem>();
    videos = new std::vector<XContentDeviceSharedItem>();
    themes = new std::vector<XContentDeviceSharedItem>();
    gamerPictures = new std::vector<XContentDeviceSharedItem>();
    avatarItems = new std::vector<XContentDeviceSharedItem>();
    systemItems = new std::vector<XContentDeviceSharedItem>();
}

XContentDevice::~XContentDevice()
{
    for (int i = 0; i < profiles->size(); i++)
    {
        for (int x = 0; x < profiles->at(i).titles.size(); x++)
            for (int y = 0; y < profiles->at(i).titles.at(x).titleSaves.size(); y++)
                delete profiles->at(i).titles.at(x).titleSaves.at(y).package;
        delete profiles->at(i).package;
    }

    CleanupSharedFiles(games);
    CleanupSharedFiles(dlc);
    CleanupSharedFiles(videos);
    CleanupSharedFiles(themes);
    CleanupSharedFiles(gamerPictures);
    CleanupSharedFiles(avatarItems);
    CleanupSharedFiles(systemItems);

    delete profiles;
    delete games;
    delete dlc;
    delete videos;
    delete themes;
    delete gamerPictures;
    delete avatarItems;
    delete systemItems;
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
        std::string profilePath = "Drive:\\Content\\Content\\" + profileFolderEntry.name + "\\FFFE07D1\\00010000\\" + profileFolderEntry.name;
        FatxFileEntry* profileEntry = drive->GetFileEntry(profilePath);

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

            // retrive all of the STFS packages in that folder
            GetAllContentItems(titleFolder, title.titleSaves);

            // there's no point in adding the title if it doesn't contain any content
            if (title.titleSaves.size() != 0)
                profile.titles.push_back(title);
        }


        profiles->push_back(profile);
    }

    // get the shared items folder
    FatxFileEntry *sharedItemsFolder = drive->GetFileEntry("Drive:\\Content\\Content\\0000000000000000");
    if (sharedItemsFolder == NULL)
        return true;

    // check for shared items
    drive->GetChildFileEntries(sharedItemsFolder);
    for (int i = 0; i < sharedItemsFolder->cachedFiles.size(); i++)
    {
        // verify that the entry is a valid title folder, should be named with title ID
        FatxFileEntry titleFolder = sharedItemsFolder->cachedFiles.at(i);
        if ((titleFolder.fileAttributes & FatxDirectory) == 0 || !ValidTitleID(titleFolder.name))
            continue;

        // get all the content items in this folder
        std::vector<XContentDeviceItem> items;
        drive->GetChildFileEntries(&titleFolder);
        GetAllContentItems(titleFolder, items);

        // put the content items in the correct category
        for (int x = 0; x < items.size(); x++)
        {
            XContentDeviceSharedItem item(items.at(x).GetPathOnDevice(), items.at(x).package);
            if (item.package == NULL)
                continue;

            switch (item.package->metaData->contentType)
            {
                case ArcadeGame:
                case CommunityGame:
                case GameOnDemand:
                case GamerTitle:
                case InstalledGame:
                case XboxOriginalGame:
                case Xbox360Title:
                    games->push_back(item);
                    break;
                case MarketPlaceContent:
                case StorageDownload:
                case XboxDownload:
                    dlc->push_back(item);
                    break;
                case GameDemo:
                    demos->push_back(item);
                    break;
                case GameTrailer:
                case GameVideo:
                case Movie:
                case MusicVideo:
                case PodcastVideo:
                case Video:
                case ViralVideo:
                    videos->push_back(item);
                    break;
                case Theme:
                    themes->push_back(item);
                    break;
                case GamerPicture:
                    gamerPictures->push_back(item);
                    break;
                case AvatarAssetPack:
                case AvatarItem:
                    avatarItems->push_back(item);
                    break;
                default:
                    systemItems->push_back(item);
                    break;
            }
        }
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

void XContentDevice::GetAllContentItems(FatxFileEntry &titleFolder, vector<XContentDeviceItem> &itemsFound)
{
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

            XContentDeviceItem item(contentPackage.path + contentPackage.name, content);

            itemsFound.push_back(item);
        }
    }
}

void XContentDevice::CleanupSharedFiles(std::vector<XContentDeviceSharedItem> *category)
{
    for (int i = 0; i < category->size(); i++)
        delete category->at(i).package;
}
