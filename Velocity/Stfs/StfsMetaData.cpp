#include "StfsMetaData.h"
#include <iostream>
#include <sstream>

using namespace std;

StfsMetaData::StfsMetaData(FileIO *io, DWORD flags) : installerType((InstallerType)0), flags(flags)
{
	// set the io
	this->io = io;

    if ((flags & MetadataSkipRead) == 0)
        readMetadata();
}

void StfsMetaData::readMetadata()
{
    // store errors thrown
    stringstream except;

    io->setPosition(0);

    if ((flags & MetadataIsPEC) == 0)
    {
        magic = (Magic)io->readDword();

        if (magic == CON)
            ReadCertificateEx(&certificate, io, 4);
        else if (magic == LIVE || magic == PIRS)
            io->readBytes(packageSignature, 0x100);
        else
        {
            except << "STFS: Content signature type 0x" << hex << (DWORD)magic << " is invalid.\n";
            throw except.str();
        }

        io->setPosition(0x22C);

        // read licensing data
        for(int i = 0; i < 0x10; i++)
        {
            UINT64 tempYo = io->readUInt64();
            licenseData[i].type = (LicenseType)(tempYo >> 48);
            licenseData[i].data = (tempYo & 0xFFFFFFFFFFFF);
            licenseData[i].bits = io->readDword();
            licenseData[i].flags = io->readDword();

            switch (licenseData[i].type)
            {
                case 0:
                case 0xFFFF:
                case 9:
                case 3:
                case 0xF000:
                case 0xE000:
                case 0xD000:
                case 0xC000:
                case 0xB000:
                    break;
                default:
                    except << "STFS: Invalid license type at index " << i << ".\n";
                    throw except.str();
            }
        }

        // header hash / content id
        io->readBytes(headerHash, 0x14);

        headerSize = io->readDword();

        // to do: make sure it's a valid type
        contentType = (ContentType)io->readDword();

        // read metadata information
        metaDataVersion = io->readDword();
        contentSize = io->readUInt64();
        mediaID = io->readDword();
        version = io->readDword();
        baseVersion = io->readDword();
        titleID = io->readDword();
        platform = io->readByte();
        executableType = io->readByte();
        discNumber = io->readByte();
        discInSet = io->readByte();
        savegameID = io->readDword();
        io->readBytes(consoleID, 5);
        io->readBytes(profileID, 8);

        // read volume descriptor
        ReadVolumeDescriptorEx(&volumeDescriptor, io, 0x379);

        dataFileCount = io->readDword();
        dataFileCombinedSize = io->readDword();

        // read the avatar metadata if needed
        if (contentType == AvatarItem)
        {
            io->setPosition(0x3D9);
            io->swapEndian();

            subCategory = (AssetSubcategory)io->readDword();
            colorizable = io->readDword();

            io->swapEndian();

            io->readBytes(guid, 0x10);
            skeletonVersion = (SkeletonVersion)io->readByte();

            if (skeletonVersion < 1 || skeletonVersion > 3)
                throw string("STFS: Invalid skeleton version.");
        }
        else if (contentType == Video) // there may be other content types with this metadata
        {
            io->setPosition(0x3D9);

            io->readBytes(seriesID, 0x10);
            io->readBytes(seasonID, 0x10);

            seasonNumber = io->readWord();
            episodeNumber = io->readWord();
        }

        // skip padding
        io->setPosition(0x3FD);

        io->readBytes(deviceID, 0x14);
        displayName = io->readWString();
        io->setPosition(0xD11);
        displayDescription = io->readWString();
        io->setPosition(0x1611);
        publisherName = io->readWString(0x80);
        io->setPosition(0x1691);
        titleName = io->readWString(0x80);
        io->setPosition(0x1711);
        transferFlags = io->readByte();

        // read image sizes
        thumbnailImageSize = io->readDword();
        titleThumbnailImageSize = io->readDword();

        thumbnailImage = new BYTE[thumbnailImageSize];
        titleThumbnailImage = new BYTE[titleThumbnailImageSize];

        // read images
        io->readBytes(thumbnailImage, thumbnailImageSize);
        io->setPosition(0x571A);

        io->readBytes(titleThumbnailImage, titleThumbnailImageSize);
        io->setPosition(0x971A);

        if (((headerSize + 0xFFF) & 0xFFFFF000) - 0x971A < 0x15F4)
            return;

        installerType = (InstallerType)io->readDword();
        switch (installerType)
        {
            case SystemUpdate:
            case TitleUpdate:
            {
                DWORD tempbv = io->readDword();
                installerBaseVersion.major = tempbv >> 28;
                installerBaseVersion.minor = (tempbv >> 24) & 0xF;
                installerBaseVersion.build = (tempbv >> 8) & 0xFFFF;
                installerBaseVersion.revision = tempbv & 0xFF;

                DWORD tempv = io->readDword();
                installerVersion.major = tempv >> 28;
                installerVersion.minor = (tempv >> 24) & 0xF;
                installerVersion.build = (tempv >> 8) & 0xFFFF;
                installerVersion.revision = tempv & 0xFF;

                break;
            }
            case SystemUpdateProgressCache:
            case TitleUpdateProgressCache:
            case TitleContentProgressCache:
            {
                resumeState = (OnlineContentResumeState)io->readDword();
                currentFileIndex = io->readDword();
                currentFileOffset = io->readUInt64();
                bytesProcessed = io->readUInt64();

                FILETIME time;
                time.dwHighDateTime = io->readDword();
                time.dwLowDateTime = io->readDword();
                lastModified = XDBFHelpers::FILETIMEtoTimeT(time);

                io->readBytes(cabResumeData, 0x15D0);
                break;
            }
            case None:
                break;
            default:
                throw string("STFS: Invalid Installer Type value.");
        }

    #ifdef DEBUG
        if(metaDataVersion != 2)
            throw string("STFS: Metadata version is not 2.\n");
    #endif
    }
    else
    {
        ReadCertificateEx(&certificate, io, 0);
        io->readBytes(headerHash, 0x14);

        ReadVolumeDescriptorEx(&volumeDescriptor, io, 0x244);

        // *skip missing int*
        io->setPosition(0x26C);
        io->readBytes(profileID, 8);
        enabled = (bool)io->readByte();
        io->readBytes(consoleID, 5);

        // anything between 1 and 0x1000 works, inclusive
        headerSize = 0x1000;
    }
}

void StfsMetaData::WriteCertificate()
{
    if (magic != CON && (flags & MetadataIsPEC) == 0)
        throw string("STFS: Error writing certificate. Package is strong signed and therefore doesn't have a certificate.\n");

    WriteCertificateEx(&certificate, io, (flags & MetadataIsPEC) ? 0 : 4);
}

void StfsMetaData::WriteMetaData()
{
    // seek to the begining of the file
    io->setPosition(0);

    if ((flags & MetadataIsPEC) == 0)
    {

        io->write(magic);

        if (magic == CON)
            WriteCertificate();
        else if (magic == PIRS || magic == LIVE)
            io->write(packageSignature, 0x100);
        else
        {
            stringstream except;
            except << "STFS: Content signature type 0x" << hex << (DWORD)magic << " is invalid.\n";
            throw except.str();
        }

        // write the licensing data
        io->setPosition(0x22C);
        for (DWORD i = 0; i < 0x10; i++)
        {
            io->write(((UINT64)licenseData[i].type << 48) | (UINT64)licenseData[i].data);
            io->write(licenseData[i].bits);
            io->write(licenseData[i].flags);
        }

        io->write(headerHash, 0x14);
        io->write(headerSize);
        io->write((DWORD)contentType);
        io->write((DWORD)metaDataVersion);
        io->write((UINT64)contentSize);
        io->write(mediaID);
        io->write(version);
        io->write(baseVersion);
        io->write(titleID);
        io->write((BYTE)platform);
        io->write((BYTE)executableType);
        io->write((BYTE)discNumber);
        io->write((BYTE)discInSet);
        io->write((DWORD)savegameID);
        io->write(consoleID, 5);
        io->write(profileID, 8);

        WriteVolumeDescriptor();

        io->write(dataFileCount);
        io->write(dataFileCombinedSize);

        // write the avatar asset metadata if needed
        if (contentType == AvatarItem)
        {
            io->setPosition(0x3D9);
            io->swapEndian();

            io->write((DWORD)subCategory);
            io->write((DWORD)colorizable);

            io->swapEndian();

            io->write(guid, 0x10);
            io->write((BYTE)skeletonVersion);
        }
        else if (contentType == Video)
        {
            io->setPosition(0x3D9);

            io->write(seriesID, 0x10);
            io->readBytes(seasonID, 0x10);

            io->write(seasonNumber);
            io->write(episodeNumber);
        }

        // skip padding
        io->setPosition(0x3FD);

        io->write(deviceID, 0x14);
        io->write(displayName);

        io->setPosition(0xD11);
        io->write(displayDescription);

        io->setPosition(0x1611);
        io->write(publisherName);

        io->setPosition(0x1691);
        io->write(titleName);

        io->setPosition(0x1711);
        io->write((BYTE)transferFlags);

        io->write(thumbnailImageSize);
        io->write(titleThumbnailImageSize);

        io->write(thumbnailImage, thumbnailImageSize);
        io->setPosition(0x571A);
        io->write(titleThumbnailImage, titleThumbnailImageSize);

        if (((headerSize + 0xFFF) & 0xFFFFF000) - 0x971A < 0x15F4)
            return;

        io->write(installerType);
        switch (installerType)
        {
            case SystemUpdate:
            case TitleUpdate:
            {
                DWORD tempbv = 0;
                tempbv |= (installerBaseVersion.major & 0xF) << 28;
                tempbv |= (installerBaseVersion.minor & 0xF) << 24;
                tempbv |= (installerBaseVersion.build & 0xFFFF) << 8;
                tempbv |= installerBaseVersion.revision & 0xFF;
                io->write(tempbv);

                DWORD tempv = 0;
                tempv |= (installerVersion.major & 0xF) << 28;
                tempv |= (installerVersion.minor & 0xF) << 24;
                tempv |= (installerVersion.build & 0xFFFF) << 8;
                tempv |= installerVersion.revision & 0xFF;
                io->write(tempv);

                break;
            }

            case SystemUpdateProgressCache:
            case TitleUpdateProgressCache:
            case TitleContentProgressCache:
            {
                io->write(resumeState);
                io->write(currentFileIndex);
                io->write(currentFileOffset);
                io->write(bytesProcessed);

                FILETIME time = XDBFHelpers::TimeTtoFILETIME(lastModified);
                io->write(time.dwHighDateTime);
                io->write(time.dwLowDateTime);

                io->write(cabResumeData, 0x15D0);
                break;
            }

            case None:
                break;
        }
    }
    else
    {
        memcpy(consoleID, certificate.ownerConsoleID, 5);

        WriteCertificateEx(&certificate, io, 0);
        io->write(headerHash, 0x14);

        WriteVolumeDescriptorEx(&volumeDescriptor, io, 0x244);

        // *skip missing int*
        io->setPosition(0x26C);
        io->write(profileID, 8);
        io->write((BYTE)enabled);
        io->write(consoleID, 5);
    }
}

void StfsMetaData::WriteVolumeDescriptor()
{
    WriteVolumeDescriptorEx(&volumeDescriptor, io, (flags & MetadataIsPEC) ? 0x244 : 0x379);
}

StfsMetaData::~StfsMetaData()
{
    if ((flags & MetadataIsPEC) == 0 && (flags & MetadataDontFreeThumbnails) == 0)
    {
        delete[] thumbnailImage;
        delete[] titleThumbnailImage;
    }
}
