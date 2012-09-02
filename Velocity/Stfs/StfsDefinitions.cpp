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
        result << ((int)(((bytes / (float)KB) + 0.05f) * 100) / 100.0f) << " KB";
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
    io->readBytes((BYTE*)tempPartNum, 0x14);
    cert->ownerConsolePartNumber = string(tempPartNum);

    cert->ownerConsoleType = (ConsoleType)io->readByte();
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
    io->write(cert->ownerConsolePartNumber.c_str(), 0x14);
    io->write((BYTE)cert->ownerConsoleType);
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
    const string names[2] = { "DevKit", "Retail" };
    return names[type - 1];
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
