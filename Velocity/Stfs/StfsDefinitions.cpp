#include "StfsDefinitions.h"


void ReadVolumeDescriptorEx(VolumeDescriptor *descriptor, FileIO *io, DWORD address)
{
    // seek to the volume descriptor
    io->setPosition(address);

    // read the descriptor
    descriptor->size = io->readByte();
    descriptor->reserved = io->readByte();
    descriptor->blockSeperation = io->readByte();

    io->setEndian(LittleEndian);

    descriptor->fileTableBlockCount = io->readWord();
    descriptor->fileTableBlockNum = io->readInt24();
    io->readBytes(descriptor->topHashTableHash, 0x14);

    io->setEndian(BigEndian);

    descriptor->allocatedBlockCount = io->readDword();
    descriptor->unallocatedBlockCount = io->readDword();
}

string LicenseTypeToString(LicenseType type)
{
    switch (type)
    {
        case Unused:
            return string("Unused");
        case Unrestricted:
            return string("Unrestricted");
        case ConsoleProfileLicense:
            return string("ConsoleProfileLicense");
        case WindowsProfileLicense:
            return string("WindowsProfileLicense");
        case ConsoleLicense:
            return string("ConsoleLicense");
        case MediaFlags:
            return string("MediaFlags");
        case KeyVaultPrivileges:
            return string("KeyVaultPrivileges");
        case HyperVisorFlags:
            return string("HyperVisorFlags");
        case UserPrivileges:
            return string("UserPrivileges");
        default:
            throw string("STFS: Invalid 'License Type' value.\n");
    }
}


string ByteSizeToString(int bytes)
{
    int B = 1; //byte
    int KB = 1024 * B; //kilobyte
    int MB = 1024 * KB; //megabyte
    int GB = 1024 * MB; //gigabyte

    // divide stuff
    std::stringstream result;
    if (bytes > GB)
        result << (bytes / GB) << " GB";
    else if (bytes > MB)
        result << (bytes / MB) << " MB";
    else if (bytes > KB)
        result << ((int)(((bytes / (float)KB) + 0.005f) * 100) / 100.0f) << " KB";
    else
        result << bytes << " bytes";

    return result.str();
}

DWORD MSTimeToDWORD(MSTime time)
{
    DWORD toReturn = 0;
    toReturn |= ((time.year - 1980) & 0xEF) << 25;
    toReturn |= (time.month & 0xF) << 21;
    toReturn |= (time.monthDay & 0x1F) << 16;
    toReturn |= (time.hours & 0x1F) << 11;
    toReturn |= (time.minutes & 0x3F) << 5;
    toReturn |= time.seconds & 0x1F;

    return toReturn;
}

MSTime DWORDToMSTime(DWORD winTime)
{
    MSTime time;

    time.year = ((winTime & 0xFE000000) >> 25) + 1980;
    time.month = (winTime & 0x1E00000) >> 21;
    time.monthDay = (winTime & 0x1F0000) >> 16;
    time.hours = (winTime & 0xF800) >> 11;
    time.minutes = (winTime & 0x7E0) >> 5;
    time.seconds = winTime & 0x1F;

    return time;
}

MSTime TimetToMSTime(time_t time)
{
    struct tm *timeInfo = localtime(&time);

    MSTime toReturn;
    toReturn.year = timeInfo->tm_year - 4;
    toReturn.month = timeInfo->tm_mon + 1;
    toReturn.monthDay = timeInfo->tm_mday;
    toReturn.hours = timeInfo->tm_hour;
    toReturn.minutes = timeInfo->tm_min;
    toReturn.seconds = timeInfo->tm_sec;

    return toReturn;
}

void WriteVolumeDescriptorEx(VolumeDescriptor *descriptor, FileIO *io, DWORD address)
{
    // volume descriptor position
    io->setPosition(address);

    // write size, padding and block seperation
    INT24 start = 0x240000;
    start |= descriptor->blockSeperation;
    io->write(start);

    // write the rest of the descriptor
    io->swapEndian();
    io->write(descriptor->fileTableBlockCount);
    io->swapEndian();

    io->write(descriptor->fileTableBlockNum, LittleEndian);
    io->write(descriptor->topHashTableHash, 0x14);
    io->write(descriptor->allocatedBlockCount);
    io->write(descriptor->unallocatedBlockCount);
}

void ReadCertificateEx(Certificate *cert, FileIO *io, DWORD address)
{
    // seek to the address of the certificate
    io->setPosition(address);

    cert->publicKeyCertificateSize = io->readWord();
    io->readBytes(cert->ownerConsoleID, 5);

    char tempPartNum[0x15];
    tempPartNum[0x14] = 0;
    io->readBytes((BYTE*)tempPartNum, 0x11);
    cert->ownerConsolePartNumber = string(tempPartNum);

    DWORD temp = io->readDword();
    cert->ownerConsoleType = (ConsoleType)(temp & 3);
    cert->consoleTypeFlags = (ConsoleTypeFlags)(temp & 0xFFFFFFFC);
    if (cert->ownerConsoleType != DevKit && cert->ownerConsoleType != Retail)
        throw string("STFS: Invalid console type.\n");

    char tempGenDate[9] = {0};
    io->readBytes((BYTE*)tempGenDate, 8);
    cert->dateGeneration = string(tempGenDate);

    cert->publicExponent = io->readDword();
    io->readBytes(cert->publicModulus, 0x80);
    io->readBytes(cert->certificateSignature, 0x100);
    io->readBytes(cert->signature, 0x80);
}

void WriteCertificateEx(Certificate *cert, FileIO *io, DWORD address)
{
    // seek to the position of the certificate
    io->setPosition(address);

    // write the certificate
    io->write(cert->publicKeyCertificateSize);
    io->write(cert->ownerConsoleID, 5);
    io->write(cert->ownerConsolePartNumber.c_str(), 0x11);
    DWORD temp = cert->consoleTypeFlags | cert->ownerConsoleType;
    io->write(temp);
    io->write(cert->dateGeneration.c_str(), 8);
    io->write(cert->publicExponent);
    io->write(cert->publicModulus, 0x80);
    io->write(cert->certificateSignature, 0x100);
    io->write(cert->signature, 0x80);
}

string MagicToString(Magic magic)
{
    switch (magic)
    {
        case CON:
            return string("CON ");
        case LIVE:
            return string("LIVE");
        case PIRS:
            return string("PIRS");
        default:
            return string("N/A");
    }
}

string ConsoleTypeToString(ConsoleType type)
{
    switch (type)
    {
        case DevKit:
            return string("DevKit");
        case Retail:
            return string("Retail");
        default:
            throw string("STFS: Invalid console type.\n");
    }
}

string ContentTypeToString(ContentType type)
{
    switch (type)
    {
        case App:
            return string("App");
        case ArcadeGame:
            return string("Arcade Game");
        case AvatarAssetPack:
        return string("Avatar Asset Pack");
        case AvatarItem:
            return string("AvatarItem");
        case CacheFile:
            return string("Cache File");
        case CommunityGame:
            return string("Community Game");
        case GameDemo:
            return string("Game Demo");
        case GamerPicture:
            return string("Gamer Picture");
        case GamerTitle:
            return string("Gamer Title");
        case GameTrailer:
            return string("Game Trailer");
        case GameVideo:
            return string("Game Video");
        case InstalledGame:
            return string("Installed Game");
        case Installer:
            return string("Installer");
        case IPTVPauseBuffer:
            return string("IPTV Pause Buffer");
        case LicenseStore:
            return string("License Store");
        case MarketPlaceContent:
            return string("Market Place Content");
        case Movie:
            return string("Movie");
        case MusicVideo:
            return string("Music Video");
        case PodcastVideo:
            return string("Podcast Video");
        case Profile:
            return string("Profile");
        case Publisher:
            return string("Publisher");
        case SavedGame:
            return string("Saved Game");
        case StorageDownload:
            return string("Storage Download");
        case Theme:
            return string("Theme");
        case Video:
            return string("Video");
        case ViralVideo:
            return string("Viral Video");
        case XboxDownload:
            return string("Xbox Download");
        case XboxOriginalGame:
            return string("Xbox Original Game");
        case XboxSavedGame:
            return string("Xbox Saved Game");
        case Xbox360Title:
            return string("Xbox360 Title");
        case XNA:
            return string("XNA");
        default:
            throw string("STFS: Invalid 'ContentType' value.\n");
    }
}
