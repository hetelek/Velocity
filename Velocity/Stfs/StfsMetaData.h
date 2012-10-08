#pragma once

#include "FileIO.h"
#include "StfsConstants.h"
#include "StfsDefinitions.h"
#include "../AvatarAsset/AvatarAssetDefinintions.h"
#include "../GPD/XDBFHelpers.h"
#include "winnames.h"
#include <iostream>

using std::string;

enum StfsMetadataFlags
{
    MetadataIsPEC = 1,
    MetadataSkipRead = 2,
    MetadataDontFreeThumbnails = 4
};

enum OnlineContentResumeState
{
    FileHeadersNotReady = 0x46494C48,
    NewFolder = 0x666F6C64,
    NewFolderResum_Attempt1 = 0x666F6C31,
    NewFolderResumeAttempt2 = 0x666F6C32,
    NewFolderResumeAttemptUnknown = 0x666F6C3F,
    NewFolderResumeAttemptSpecific = 0x666F6C40
};

class StfsMetaData
{
public:
	// Description: read in all of the metadata for the package
    StfsMetaData(FileIO *io, DWORD flags = 0);

    // Description: write the console certificate
    void WriteCertificate();

    // Description: write the volume descriptor
	void WriteVolumeDescriptor();

    // Description: write all of the metadata
    void WriteMetaData();
	~StfsMetaData();

    Magic magic;

	// only console signed
    Certificate certificate;

	// only strong signed
	BYTE packageSignature[0x100];

	LicenseEntry licenseData[0x10];
	BYTE headerHash[0x14];
	DWORD headerSize;
	ContentType contentType;
	DWORD metaDataVersion;
	UINT64 contentSize;
	DWORD mediaID;
	DWORD version;
	DWORD baseVersion;
	DWORD titleID;
	BYTE platform;
	BYTE executableType;
	BYTE discNumber;
	BYTE discInSet;
	DWORD savegameID;
	BYTE consoleID[5];
    BYTE profileID[8];

	VolumeDescriptor volumeDescriptor;

	// metadata v1
	DWORD dataFileCount;
	UINT64 dataFileCombinedSize;
	BYTE deviceID[0x14];
	wstring displayName;
	wstring displayDescription;
	wstring publisherName;
	wstring titleName;
    BYTE transferFlags;
	DWORD thumbnailImageSize;
	DWORD titleThumbnailImageSize;

    // credit to Eaton for all the extra metadata stuff
    InstallerType installerType;

    // Avatar Asset
    AssetSubcategory subCategory;
    DWORD colorizable;
    BYTE guid[0x10];
    SkeletonVersion skeletonVersion;

    // media
    BYTE seriesID[0x10];
    BYTE seasonID[0x10];
    WORD seasonNumber;
    WORD episodeNumber;

    // installer progress cache data
    OnlineContentResumeState resumeState;
    DWORD currentFileIndex;
    UINT64 currentFileOffset;
    UINT64 bytesProcessed;
    time_t lastModified;
    BYTE cabResumeData[5584];

    // installer update data
    Version installerBaseVersion;
    Version installerVersion;

	BYTE *thumbnailImage;
	BYTE *titleThumbnailImage;

private:
	FileIO *io;
    DWORD flags;

    void readMetadata();
};

