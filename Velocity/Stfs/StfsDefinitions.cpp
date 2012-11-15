#include "StfsDefinitions.h"

#include <iostream>
#include <sstream>

#include <QString>

#include "../FileIO.h"

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

QString LicenseTypeToString(LicenseType type)
{
    switch (type)
    {
        case Unused:
            return QString("Unused");
        case Unrestricted:
            return QString("Unrestricted");
        case ConsoleProfileLicense:
            return QString("ConsoleProfileLicense");
        case WindowsProfileLicense:
            return QString("WindowsProfileLicense");
        case ConsoleLicense:
            return QString("ConsoleLicense");
        case Unknown1:
            return QString("Unknown1");
        case Unknown2:
            return QString("Unknown2");
        case Unknown3:
            return QString("Unknown3");
        case Unknown4:
            return QString("Unknown4");
        default:
            throw QString("STFS: Invalid 'License Type' value.\n");
    }
}


QString ByteSizeToString(int bytes)
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

    return QString::fromStdString(result.str());
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
    cert->ownerConsolePartNumber = QString(tempPartNum);

    DWORD temp = io->readDword();
    cert->ownerConsoleType = (ConsoleType)(temp & 3);
    cert->consoleTypeFlags = (ConsoleTypeFlags)(temp & 0xFFFFFFFC);
    if (cert->ownerConsoleType != DevKit && cert->ownerConsoleType != Retail)
        throw QString("STFS: Invalid console type.\n");

    char tempGenDate[9] = {0};
    io->readBytes((BYTE*)tempGenDate, 8);
    cert->dateGeneration = QString(tempGenDate);

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
    io->write((const char *) cert->ownerConsolePartNumber.toLatin1().constData(), 0x11);
    DWORD temp = cert->consoleTypeFlags | cert->ownerConsoleType;
    io->write(temp);
    io->write((const char *) cert->dateGeneration.toLatin1().constData(), 8);
    io->write(cert->publicExponent);
    io->write(cert->publicModulus, 0x80);
    io->write(cert->certificateSignature, 0x100);
    io->write(cert->signature, 0x80);
}

QString MagicToString(Magic magic)
{
    switch (magic)
    {
        case CON:
            return QString("CON ");
        case LIVE:
            return QString("LIVE");
        case PIRS:
            return QString("PIRS");
        default:
            return QString("N/A");
    }
}

QString ConsoleTypeToString(ConsoleType type)
{
    switch (type)
    {
        case DevKit:
            return QString("DevKit");
        case Retail:
            return QString("Retail");
        default:
            throw QString("STFS: Invalid console type.\n");
    }
}

QString ContentTypeToString(ContentType type)
{
    switch (type)
    {
        case App:
            return QString("App");
        case ArcadeGame:
            return QString("Arcade Game");
        case AvatarAssetPack:
            return QString("Avatar Asset Pack");
        case AvatarItem:
            return QString("AvatarItem");
        case CacheFile:
            return QString("Cache File");
        case CommunityGame:
            return QString("Community Game");
        case GameDemo:
            return QString("Game Demo");
        case GamerPicture:
            return QString("Gamer Picture");
        case GamerTitle:
            return QString("Gamer Title");
        case GameTrailer:
            return QString("Game Trailer");
        case GameVideo:
            return QString("Game Video");
        case InstalledGame:
            return QString("Installed Game");
        case Installer:
            return QString("Installer");
        case IPTVPauseBuffer:
            return QString("IPTV Pause Buffer");
        case LicenseStore:
            return QString("License Store");
        case MarketPlaceContent:
            return QString("Market Place Content");
        case Movie:
            return QString("Movie");
        case MusicVideo:
            return QString("Music Video");
        case PodcastVideo:
            return QString("Podcast Video");
        case Profile:
            return QString("Profile");
        case Publisher:
            return QString("Publisher");
        case SavedGame:
            return QString("Saved Game");
        case StorageDownload:
            return QString("Storage Download");
        case Theme:
            return QString("Theme");
        case Video:
            return QString("Video");
        case ViralVideo:
            return QString("Viral Video");
        case XboxDownload:
            return QString("Xbox Download");
        case XboxOriginalGame:
            return QString("Xbox Original Game");
        case XboxSavedGame:
            return QString("Xbox Saved Game");
        case Xbox360Title:
            return QString("Xbox360 Title");
        case XNA:
            return QString("XNA");
        default:
            throw QString("STFS: Invalid 'ContentType' value.\n");
    }
}
