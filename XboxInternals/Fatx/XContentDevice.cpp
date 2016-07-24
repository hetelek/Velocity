#include "XContentDevice.h"

XContentDevice::XContentDevice(FatxDrive *drive) :
    drive(drive), content(NULL)
{
    profiles = new std::vector<XContentDeviceProfile>();

    games = new std::vector<XContentDeviceSharedItem>();
    dlc = new std::vector<XContentDeviceSharedItem>();
    demos = new std::vector<XContentDeviceSharedItem>();
    videos = new std::vector<XContentDeviceSharedItem>();
    themes = new std::vector<XContentDeviceSharedItem>();
    gamerPictures = new std::vector<XContentDeviceSharedItem>();
    avatarItems = new std::vector<XContentDeviceSharedItem>();
    updates = new std::vector<XContentDeviceSharedItem>();
    systemItems = new std::vector<XContentDeviceSharedItem>();
}

XContentDevice::~XContentDevice()
{
    for (int i = 0; i < profiles->size(); i++)
    {
        for (int x = 0; x < profiles->at(i).titles.size(); x++)
            for (int y = 0; y < profiles->at(i).titles.at(x).titleSaves.size(); y++)
                delete profiles->at(i).titles.at(x).titleSaves.at(y).content;
        delete profiles->at(i).content;
    }

    CleanupSharedFiles(games);
    CleanupSharedFiles(dlc);
    CleanupSharedFiles(videos);
    CleanupSharedFiles(themes);
    CleanupSharedFiles(gamerPictures);
    CleanupSharedFiles(avatarItems);
    CleanupSharedFiles(updates);
    CleanupSharedFiles(systemItems);

    delete profiles;
    delete games;
    delete dlc;
    delete videos;
    delete themes;
    delete gamerPictures;
    delete avatarItems;
    delete updates;
    delete systemItems;
}

bool XContentDevice::LoadDevice(void(*progress)(void*, bool), void *arg)
{
    // the generic path for content is:
    // Drive:\Content\Content\OFFLINE_XUID\TITLE_ID\CONTENT_TYPE\STFS_PACKAGE

    // locate the content partition
    std::vector<Partition*> partitions = drive->GetPartitions();
    for (int i = 0; i < partitions.size(); i++)
    {
        if (partitions.at(i)->name == "Content")
        {
            content = partitions.at(i);
            break;
        }
    }

    if (content == NULL)
    {
        if(progress)
            progress(arg, true);
        return false;
    }

    GetFreeMemory(progress, arg, false);

    FatxFileEntry *fileEntry = drive->GetFileEntry("Drive:\\Content\\Content\\");
    if (fileEntry == NULL)
    {
        // if the content folder doesn't exist, then create i;t
        drive->CreatePath("Drive:\\Content\\Content\\");

        if (progress)
            progress(arg, true);
        return true;
    }

    // load all of the sub dirents in Drive:\Content\Content
    drive->GetChildFileEntries(fileEntry);

    // check for profile folders
    for (int i = 0; i< fileEntry->cachedFiles.size(); i++)
    {
        FatxFileEntry profileFolderEntry = fileEntry->cachedFiles.at(i);

        // all profile folders are named the profile's offline XUID
        if ((profileFolderEntry.fileAttributes & FatxDirectory) == 0 || !ValidOfflineXuid(profileFolderEntry.name) || profileFolderEntry.nameLen == 0xE5)
            continue;

        // check for a profile file
        std::string profilePath = "Drive:\\Content\\Content\\" + profileFolderEntry.name + "\\FFFE07D1\\00010000\\" + profileFolderEntry.name;
        FatxFileEntry* profileEntry = drive->GetFileEntry(profilePath);

        StfsPackage *profilePackage = NULL;
        std::string rawName = "";
        if (profileEntry != NULL)
        {
            try
            {
                FatxIO io = drive->GetFatxIO(profileEntry);
                profilePackage = new StfsPackage(new FatxIO(io), StfsPackageDeleteIO);
                rawName = profileEntry->name;
            }
            catch (...)
            {
                profilePath = "";
                profilePackage = NULL;
            }
        }
        else
        {
            profilePath = "";
        }
        XContentDeviceProfile profile(profilePath, profileFolderEntry.name, profilePackage, (profileEntry != NULL) ? profileEntry->fileSize : 0);

        // get all of the game save data for this profile
        drive->GetChildFileEntries(&profileFolderEntry);
        for (int x = 0; x < profileFolderEntry.cachedFiles.size(); x++)
        {
            // verify that the entry is a valid title folder, should be named with title ID
            FatxFileEntry titleFolder = profileFolderEntry.cachedFiles.at(x);
            if ((titleFolder.fileAttributes & FatxDirectory) == 0 || !ValidTitleID(titleFolder.name) || titleFolder.name == "FFFE07D1" || titleFolder.nameLen == 0xE5)
                continue;

            XContentDeviceTitle title(titleFolder.path + "\\" + titleFolder.name, titleFolder.name);

            // load all of the sub folders
            drive->GetChildFileEntries(&titleFolder);

            // retrive all of the STFS packages in that folder
            GetAllContentItems(titleFolder, title.titleSaves, progress, arg);

            // there's no point in adding the title if it doesn't contain any content
            if (title.titleSaves.size() != 0)
                profile.titles.push_back(title);

            // update progress if needed
            if (progress)
                progress(arg, false);
        }

        if (profile.titles.size() != 0 || profile.content != NULL)
            profiles->push_back(profile);
    }

    // get the shared items folder
    FatxFileEntry *sharedItemsFolder = drive->GetFileEntry("Drive:\\Content\\Content\\0000000000000000");
    if (sharedItemsFolder == NULL)
    {
        if(progress)
            progress(arg, true);
        return true;
    }

    // check for shared items
    drive->GetChildFileEntries(sharedItemsFolder);
    for (int i = 0; i < sharedItemsFolder->cachedFiles.size(); i++)
    {
        // verify that the entry is a valid title folder, should be named with title ID
        FatxFileEntry titleFolder = sharedItemsFolder->cachedFiles.at(i);
        if ((titleFolder.fileAttributes & FatxDirectory) == 0 || !ValidTitleID(titleFolder.name) || titleFolder.nameLen == 0xE5)
            continue;

        // get all the content items in this folder
        std::vector<XContentDeviceItem> items;
        drive->GetChildFileEntries(&titleFolder);
        GetAllContentItems(titleFolder, items, progress, arg);

        // put the content items in the correct category
        for (int x = 0; x < items.size(); x++)
        {
            // update progress if needed
            if (progress)
                progress(arg, false);

            // XContentDeviceSharedItem needs the size of just the root file descriptor for SVOD systems so we need to subtract
            // off the size of all the data files, this doesn't affect STFS packages at all
            DWORD rootFileSize = items.at(x).GetFileSize() - items.at(x).content->metaData->dataFileCombinedSize;
            XContentDeviceSharedItem item(items.at(x).GetPathOnDevice(), items.at(x).GetRawName(),
                                          items.at(x).content, rootFileSize, items.at(x).GetContentFilePaths());
            if (item.content == NULL)
                continue;

            switch (item.content->metaData->contentType)
            {
                case ArcadeGame:
                case CommunityGame:
                case GameOnDemand:
                case GamerTitle:
                case InstalledGame:
                case XboxOriginalGame:
                case Xbox360Title:
                case IndieGame:
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
            case Installer:
                    updates->push_back(item);
                    break;
                default:
                    systemItems->push_back(item);
                    break;
            }
        }
    }

    if(progress)
        progress(arg, true);
    return true;
}

FatxDriveType XContentDevice::GetDeviceType()
{
    return drive->GetFatxDriveType();
}

UINT64 XContentDevice::GetFreeMemory(void(*progress)(void*, bool), void *arg, bool finish)
{
    drive->GetFreeMemory(content, progress, arg, finish);
    return content->freeMemory;
}

UINT64 XContentDevice::GetTotalMemory()
{
    return (UINT64)content->clusterCount * (UINT64)content->clusterSize;
}

std::wstring XContentDevice::GetName()
{
    FatxFileEntry *nameEntry = drive->GetFileEntry("Drive:\\Content\\name.txt");
    std::wstring name = (GetDeviceType() == FatxHarddrive) ? L"Hard Drive" : L"Flash Drive";
    if (nameEntry)
    {
        FatxIO nameFile = drive->GetFatxIO(nameEntry);
        nameFile.SetPosition(0);

        // make sure that it starts with 0xFEFF
        if (nameFile.ReadWord() == 0xFEFF)
            name = nameFile.ReadWString((nameEntry->fileSize > 0x36) ? 26 : (nameEntry->fileSize - 2) / 2);
    }

    return name;
}

void XContentDevice::SetName(std::wstring name)
{
    drive->SetDriveName(name);
}

FatxDrive *XContentDevice::GetFatxDrive()
{
    return drive;
}

void XContentDevice::CopyFileToLocalDisk(std::string outPath, std::string inPath, void (*progress)(void *, DWORD, DWORD), void *arg)
{
    FatxIO io = drive->GetFatxIO(drive->GetFileEntry(inPath));
    io.SaveFile(outPath, progress, arg);
}

void XContentDevice::CopyFileToDevice(std::string outPath, void (*progress)(void *, DWORD, DWORD), void *arg)
{
    // get the file system type
    FileSystem fileSystem = IXContentHeader::GetFileSystem(outPath);

    // the content file must be destroyed so that it will close the files it's using so that it can be opened again
    // to be written to the device
    std::string devicePath, profileID, titleID;
    {
        IXContentHeader contentFile;
        if (fileSystem == FileSystemSTFS)
            contentFile = StfsPackage(outPath, StfsPackageDontReadFileListing);
        else
            contentFile = SVOD(outPath, NULL, false);

        devicePath = contentFile.GetFatxFilePath();

        profileID = Utils::ConvertToHexString((BYTE*)contentFile.metaData->profileID, 8);
        titleID = Utils::ConvertToHexString(contentFile.metaData->titleID);
    }

    // if the parent entry doesn't exist, then we need to create it
    FatxFileEntry *parent = drive->GetFileEntry(devicePath);
    if (parent == NULL)
        parent = drive->CreatePath(devicePath);

    char *tmp = new char[outPath.size() + 1];
    memcpy(tmp, outPath.c_str(), outPath.size() + 1);
#ifdef __WIN32
    PathStripPathA(tmp);
    std::string fileName(tmp);
#else
    std::string fileName(basename(tmp));
#endif
    delete tmp;

    drive->InjectFile(parent, fileName, outPath, progress, arg);

    // open the package on the device
    FatxFileEntry *fileEntry = drive->GetFileEntry(devicePath + fileName);

    IXContentHeader *contentOnDevice;
    if (fileSystem == FileSystemSTFS)
        contentOnDevice = new StfsPackage(new FatxIO(drive->GetFatxIO(fileEntry)), StfsPackageDeleteIO);
    else
        contentOnDevice = new SVOD(devicePath + fileName, drive, false);

    BYTE sharedProfileID[8] = { 0 };

    if (contentOnDevice->metaData->contentType == Profile)
    {
        // if there are already saves on this device under this profile, then we need to put it in the correct spot
        for (size_t i = 0; i < profiles->size(); i++)
        {
            if (memcmp(profiles->at(i).GetProfileID(), contentOnDevice->metaData->profileID, 8) == 0)
            {
                profiles->at(i).content = contentOnDevice;
                return;
            }
        }

        // if the profile is entirely new, then just add it to the end
        XContentDeviceProfile profile(fileEntry, static_cast<StfsPackage*>(contentOnDevice));
        profiles->push_back(profile);
    }
    else if (memcmp(contentOnDevice->metaData->profileID, sharedProfileID, 8) != 0)
    {
        XContentDeviceItem item(fileEntry, contentOnDevice);

        // check to see if the owner of this save's profile is already on the device
        for (size_t i = 0; i < profiles->size(); i++)
        {
            // I'm not sure why I have to do it this way, but if I don't,
            // I get a segmentation fault on the call to GetProfileID()
            XContentDeviceProfile prof = profiles->at(i);
            BYTE *tempID = prof.GetProfileID();

            if (memcmp(tempID, contentOnDevice->metaData->profileID, 8) == 0)
            {
                // check and see if this profile already has content under this title
                for (size_t x = 0; x < profiles->at(i).titles.size(); x++)
                {
                    XContentDeviceTitle *titleP = &profiles->at(i).titles.at(x);
                    // if the title already exists, then just add this save to the title's list of content
                    if (titleP->GetTitleID() == contentOnDevice->metaData->titleID)
                    {
                        titleP->titleSaves.push_back(item);
                        return;
                    }
                }

                // if the title isn't on the profile, then we need to create another title
                XContentDeviceTitle title("Drive:\\Content\\Content\\" + profileID + "\\" + titleID + "\\", titleID);
                title.titleSaves.push_back(item);
                profiles->at(i).titles.push_back(title);

                return;
            }
        }

        // if the profile doesn't exist, then we need to create another one
        XContentDeviceProfile profile("Drive:\\Content\\Content\\" + profileID + "\\", profileID, NULL);
        XContentDeviceTitle title("Drive:\\Content\\Content\\" + profileID + "\\" + titleID + "\\", titleID);
        title.titleSaves.push_back(item);
        profile.titles.push_back(title);

        // add the new profile to the device
        profiles->push_back(profile);
    }
    // it must be a shared item
    else
    {
        XContentDeviceSharedItem item(fileEntry, contentOnDevice);

        // put the shared item in the correct category
        switch (item.content->metaData->contentType)
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

void XContentDevice::CopyFileToRawDevice(std::string outPath, std::string name, std::string inPath, void (*progress)(void *, DWORD, DWORD), void *arg)
{
    // make the directory if it doesn't exist
    if (!drive->FileExists(inPath))
        drive->CreatePath(inPath);

    FatxFileEntry *directory = drive->GetFileEntry(inPath);
    drive->InjectFile(directory, name, outPath, progress, arg);
}

void XContentDevice::DeleteFile(IXContentHeader *package, std::string pathOnDevice)
{
    // remove the file from the hierarchy
    BYTE nullID[8] = { 0 };

    // find the content in memory to
    // file is from a profile, so it's not shared
    if ((memcmp(package->metaData->profileID, nullID, 8)) != 0)
    {
        for (int i = 0; i < profiles->size(); i++)
        {
            if (profiles->at(i).GetProfileID() && memcmp(profiles->at(i).GetProfileID(), package->metaData->profileID, 8) == 0)
            {
                for (int x = 0; x < profiles->at(i).titles.size(); x++)
                {
                    if (profiles->at(i).titles.at(x).GetTitleID() == package->metaData->titleID)
                    {
                        for (int y = 0; y < profiles->at(i).titles.at(x).titleSaves.size(); y++)
                        {
                            if (profiles->at(i).titles.at(x).titleSaves.at(y).content == package)
                            {
                                profiles->at(i).titles.at(x).titleSaves.erase(profiles->at(i).titles.at(x).titleSaves.begin() + y);
                                delete package;
                            }
                        }

                        // if there aren't any more saves for the title then remove the title
                        if (profiles->at(i).titles.at(x).titleSaves.size() == 0)
                            profiles->at(i).titles.erase(profiles->at(i).titles.begin() + x);
                    }
                }

                // if the profile doesn't have any more content in it and it's an "Unknown Profile" remove it
                if (profiles->at(i).titles.size() == 0 && profiles->at(i).GetName() == L"Unknown Profile")
                    profiles->erase(profiles->begin() + i);
            }
        }
    }
    // file is shared
    else
    {
        std::vector<XContentDeviceSharedItem> *sharedItemCategory;
        switch (package->metaData->contentType)
        {
            case ArcadeGame:
            case CommunityGame:
            case GameOnDemand:
            case GamerTitle:
            case InstalledGame:
            case XboxOriginalGame:
            case Xbox360Title:
                sharedItemCategory = games;
                break;
            case MarketPlaceContent:
            case StorageDownload:
            case XboxDownload:
                sharedItemCategory = dlc;
                break;
            case GameDemo:
                sharedItemCategory = demos;
                break;
            case GameTrailer:
            case GameVideo:
            case Movie:
            case MusicVideo:
            case PodcastVideo:
            case Video:
            case ViralVideo:
                sharedItemCategory = videos;
                break;
            case Theme:
                sharedItemCategory = themes;
                break;
            case GamerPicture:
                sharedItemCategory = gamerPictures;
                break;
            case AvatarAssetPack:
            case AvatarItem:
                sharedItemCategory = avatarItems;
                break;
            default:
                sharedItemCategory = systemItems;
                break;
        }

        for (int i = 0; i < sharedItemCategory->size(); i++)
        {
            if (sharedItemCategory->at(i).content == package)
            {
                sharedItemCategory->erase(sharedItemCategory->begin() + i);
                delete package;
            }
        }
    }

    // actually delete it from the device
    drive->RemoveFile(drive->GetFileEntry(pathOnDevice));
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

void XContentDevice::GetAllContentItems(FatxFileEntry &titleFolder, vector<XContentDeviceItem> &itemsFound, void(*progress)(void *, bool), void *arg)
{
    // iterate through all of the content types for this title
    for (int y = 0; y < titleFolder.cachedFiles.size(); y++)
    {
        // verify that the entry is a folder and named with the content type as a string in hex,
        // so for savegames the folder would be named 00000001
        FatxFileEntry contentTypeFolder = titleFolder.cachedFiles.at(y);
        if ((contentTypeFolder.fileAttributes & FatxDirectory) == 0 || !ValidTitleID(contentTypeFolder.name) || contentTypeFolder.nameLen == 0xE5)
            continue;

        // load all of the content items in this directory
        drive->GetChildFileEntries(&contentTypeFolder);

        // iterate through all of the STFS packages in this content type folder
        for (int z = 0; z < contentTypeFolder.cachedFiles.size(); z++)
        {
            // we're looking for STFS packages, so make sure the entry isn't another directory
            FatxFileEntry *contentPackage = &contentTypeFolder.cachedFiles.at(z);
            if (contentPackage->fileAttributes & FatxDirectory || contentPackage->nameLen == 0xE5)
                continue;

            FatxFileEntry *entry = drive->GetFileEntry(contentPackage->path + contentPackage->name);
            drive->GetFileEntryMagic(entry);
            DWORD fileMagic = entry->magic;

            // verify the magic
            if (fileMagic != CON && fileMagic != LIVE && fileMagic != PIRS)
                continue;

            // get the type, SVOD or STFS
            FileSystem fileSystem = entry->fileSystem;

            if (progress)
                progress(arg, false);

            // this might not be a valid STFS or SVOD package, so we have to do try {} catch {}
            IXContentHeader *content;
            std::vector<std::string> contentFilePaths;
            try
            {
                FatxIO io = drive->GetFatxIO(entry);

                if (fileSystem == FileSystemSTFS)
                {
                    content = new StfsPackage(new FatxIO(io), StfsPackageDeleteIO | StfsPackageDontReadFileListing);
                }
                else if (fileSystem == FileSystemSVOD)
                {
                    std::string rootFilePath = entry->path + entry->name;
                    content = new SVOD(rootFilePath, drive, false);

                    // SVOD systems have data files where the actual content is stored; they're in a folder in the same
                    // directory as the header file with the name {HEADER_FILE_NAME}.data
                    FatxFileEntry *dataFileDirectory = drive->GetFileEntry(rootFilePath + ".data");
                    drive->GetChildFileEntries(dataFileDirectory);
                    for (size_t i = 0; i < dataFileDirectory->cachedFiles.size(); i++)
                    {
                        // skip over deleted files
                        FatxFileEntry curEntry = dataFileDirectory->cachedFiles.at(i);
                        if (curEntry.nameLen == FATX_ENTRY_DELETED)
                            continue;

                        std::string dataFilePath = curEntry.path + curEntry.name;
                        contentFilePaths.push_back(dataFilePath);
                    }
                }
            }
            catch (...)
            {
                continue;
            }

            XContentDeviceItem item(contentPackage, content, contentFilePaths);

            itemsFound.push_back(item);

            if (progress)
                progress(arg, false);
        }
    }
}

void XContentDevice::CleanupSharedFiles(std::vector<XContentDeviceSharedItem> *category)
{
    for (int i = 0; i < category->size(); i++)
        delete category->at(i).content;
}

std::string XContentDevice::ToUpper(std::string str)
{
    std::string toReturn;
    for (int i = 0; i < str.size(); i++)
        toReturn += toupper(str.at(i));
    return toReturn;
}
