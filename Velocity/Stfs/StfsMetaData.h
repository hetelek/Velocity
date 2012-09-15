#pragma once

#include "FileIO.h"
#include "StfsConstants.h"
#include "StfsDefinitions.h"
#include <iostream>

using std::string;

enum StfsMetadataFlags
{
    MetadataIsPEC = 1,
    MetadataSkipRead = 2,
    MetadataDontFreeThumbnails = 4
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

	// metadata v2
	BYTE seriesID[0x10];
	BYTE seasonID[0x10];
	WORD seasonNumber;
	WORD episodeNumber;
	wstring additionalDisplayNames;
	wstring additionalDisplayDescriptions;

	BYTE *thumbnailImage;
	BYTE *titleThumbnailImage;

private:
	FileIO *io;
    DWORD flags;

    void readMetadata();
};

