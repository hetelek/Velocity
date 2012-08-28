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
            return string("ArcadeGame");
        case AvatarItem:
            return string("AvatarItem");
        case CacheFile:
            return string("CacheFile");
        case CommunityGame:
            return string("CommunityGame");
        case GameDemo:
            return string("GameDemo");
        case GamerPicture:
            return string("GamerPicture");
        case GamerTitle:
            return string("GamerTitle");
        case GameTrailer:
            return string("GameTrailer");
        case GameVideo:
            return string("GameVideo");
        case InstalledGame:
            return string("InstalledGame");
        case Installer:
            return string("Installer");
        case IPTVPauseBuffer:
            return string("IPTVPauseBuffer");
        case LicenseStore:
            return string("LicenseStore");
        case MarketPlaceContent:
            return string("MarketPlaceContent");
        case Movie:
            return string("Movie");
        case MusicVideo:
            return string("MusicVideo");
        case PodcastVideo:
            return string("PodcastVideo");
        case Profile:
            return string("Profile");
        case Publisher:
            return string("Publisher");
        case SavedGame:
            return string("SavedGame");
        case StorageDownload:
            return string("StorageDownload");
        case Theme:
            return string("Theme");
        case Video:
            return string("Video");
        case ViralVideo:
            return string("ViralVideo");
        case XboxDownload:
            return string("XboxDownload");
        case XboxOriginalGame:
            return string("XboxOriginalGame");
        case XboxSavedGame:
            return string("XboxSavedGame");
        case Xbox360Title:
            return string("Xbox360Title");
        case XNA:
            return string("XNA");
        default:
            throw string("STFS: Invalid 'ContentType' value.\n");
    }
}
