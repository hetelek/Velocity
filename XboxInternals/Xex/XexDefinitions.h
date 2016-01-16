#ifndef XEXDEFINITIONS_H
#define XEXDEFINITIONS_H

#include "winnames.h"
#include "Stfs/StfsDefinitions.h"

#include <string>
#include <vector>

#define XEX_HEADER_MAGIC 					0x58455832 		// 'XEX2'
#define XEX_HEADER_SIZE  					0x18
#define XEX_OPTIONAL_HEADER_ENTRY_SIZE		0x8
#define XEX_STATIC_LIBRARY_ENTRY_SIZE		0x10
#define XEX_RESOURCE_FILE_ENTRY_SIZE		0x10
#define XEX_AES_BLOCK_SIZE					0x10
#define XEX_LAN_KEY_SIZE					0x10
#define XEX_COMPRESSION_BLOCK_SIZE			0x8
#define XEX_COPY_BUFFER_SIZE				0x100000

const BYTE XEX_RETAIL_KEY[XEX_AES_BLOCK_SIZE] = { 0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28, 0xFD, 0xC3,
                                              0x40, 0x58, 0x3F, 0xBB, 0x08, 0x96, 0xBF, 0x91 };

// this key is wrong
const BYTE XEX_DEVKIT_KEY[XEX_AES_BLOCK_SIZE] = { 0xA2, 0x6C, 0x10, 0xF7, 0x1F, 0xD9, 0x35, 0xE9,
                                              0x8B, 0x99, 0x92, 0x2C, 0xE9, 0x32, 0x15 ,0x72};

enum XexOptionHeaderEntryID
{
    ResourceInfo = 0x2FF,					// out
    BaseFileDescriptor = 0x3FF,				// out
    SystemImportLibraries = 0x103FF,		// out
    StaticLibraries = 0x200FF,				// out
    ImageBaseAddress = 0x10201,				// in
    OriginalBaseAddress = 0x10001,			// in
    EntryPoint = 0x10100,					// in
    ChecksumInformation = 0x18002,			// out
    TLSData = 0x20104,						// out
    DefaultStackSize = 0x20200,				// in
    DefaultFileSystemCacheSize = 0x20301,	// in
    DefaultHeapSize = 0x20401,				// in
    PageHeapData = 0x28002,					// out
    TitleWorkspaceSize = 0x40201,			// in
    CallcapInformation = 0x18102,			// out, not sure how long this is but if the entry exists then the "Image is enabled for callcap"
    FastcapInformation = 0x18200,			// out, not sure how long this is but if the entry exists then the "Image is enabled for fastcap"
    OriginalPEImageName = 0x183FF,			// out
    Xbox360Logo = 0x405FF,					// out, not sure on the format of this, most likely a variation of a bitmap
    ExecutionInfo = 0x40006,
    RatingInformation = 0x40310,
    LANKey = 0x40404
};

enum XexModuleFlag
{
    ModuleTitle = 1,
    ModuleExportsToTitle = 2,
    ModuleSystemDebugger = 4,
    ModuleDLL = 8,
    ModulePatch = 16,
    ModulePatchFull = 32,
    ModulePatchDelta = 64,
    ModuleUserMode = 128
};

enum XexAllowedMediaType
{
    HardDisk = 1,
    DVDX2 = 2,
    DVDCD = 4,
    DVD5 = 8,
    DVD9 = 0x10,
    SystemFlash = 0x20,
    MemoryUnit = 0x80,
    MassStorageDevice = 0x100,
    SMBFileSystem = 0x200,
    DirectFromRAM = 0x400,
    SecureVirtualOpticalDevice = 0x1000,
    WirelessNStorageDevice = 0x2000,
    SystemExtendedPartition = 0x4000,
    SystemAuxillaryPartition = 0x8000,
    InsecurePackage = 0x10000,
    SaveGamePackage = 0x20000,
    LocallySignedPackage = 0x40000,
    LiveSignedPackage = 0x80000,
    XboxPlatformPackage = 0x100000
};

enum XexImageFlag
{
    ManufacturingType = 6,					// 0 is nothing, 2 is ManufacturingUtility, 4 is ManufacturingSupportTool, 6 is ManufacturingAwareModule
    XGD2MediaOnly = 8,
    CardeaKey = 0x100,
    XeikaKey = 0x200,
    TitleUserMode = 0x400,
    SystemUserMode = 0x800,
    Orange0 = 0x1000,
    Orange1 = 0x2000,
    Orange2 = 0x4000,
    IPTVSignupApplication = 0x10000,
    IPTVTitleApplication = 0x20000,
    KeyvaultPrivilegesRequired = 0x4000000,
    OnlineActivationRequired = 0x8000000,
    PageSize4KB = 0x10000000,				// if not then the page size is 64KB
    NoGameRegion = 0x20000000,
    RevocationCheckOptional = 0x40000000,
    RevocationCheckRequired = 0x80000000
};

enum XexRegion
{
    XexRegionNorthAmerica = 0xFF,
    XexRegionJapan = 0x100,
    XexRegionChina = 0x200,
    XexRegionRestOfAsia = 0xFC00,
    XexRegionAustraliaNewZealand = 0x10000,
    XexRegionRestOfEurope = 0xFE0000,
    XexRegionRestOfWorld = 0xFF000000
};

enum ESRBRating
{
    ESRB_EC = 0,
    ESRB_E = 2,
    ESRB_10 = 4,
    ESRB_T = 6,
    ESRB_M = 8,
    ESRB_RP = 0xFF
};

enum PEGIRating
{
    PEGI_3 = 0,
    PEGI_7 = 4,
    PEGI_12 = 9,
    PEGI_16 = 0xD,
    PEGI_18 = 0xE,
    PEGI_Unrated = 0xFF
};

enum PEGIFIRating
{
    PEGIFI_3 = 0,
    PEGIFI_7 = 4,
    PEGIFI_11 = 8,
    PEGIFI_15 = 0xC,
    PEGIFI_18 = 0xE,
    PEGIFI_Unrated = 0xFF
};

enum PEGIPTRating
{
    PEGIPT_4 = 1,
    PEGIPT_6 = 3,
    PEGIPT_12 = 9,
    PEGIPT_16 = 0xD,
    PEGIPT_18 = 0xE,
    PEGIPT_Unrated = 0xFF
};

enum PEGIBBFCRating
{
    PEGIBBFC_3 = 0,
    PEGIBBFC_4 = 1,
    PEGIBBFC_7 = 4,
    PEGIBBFC_8 = 5,
    PEGIBBFC_12 = 9,
    PEGIBBFC_15 = 0xC,
    PEGIBBFC_16 = 0xD,
    PEGIBBFC_18 = 0xE,
    PEGIBBF_Unrated = 0xFF
};

enum OFLCAURating
{
    OFLCAU_G = 0,
    OFLCAU_PG = 3,
    OFLCAU_M = 4,
    OFLCAU_MA15_PLUS = 6,
    OFLCAU_UNRATED = 0xFF
};

enum OFLCNZRating
{
    OFLCNZ_G = 0,
    OFLCNZ_PG = 2,
    OFLCNZ_M = 4,
    OFLCNZ_MA15_PLUS = 6,
    OFLCNZ_R16 = 0x20,
    OFLCNZ_UNRATED = 0xFF
};

enum XexCompressionState
{
    XexDecompressed = 0,
    XexCompressed,
    XexSupercompressed
};

struct XexHeader
{
    DWORD magic;
    DWORD moduleFlags;
    DWORD dataAddress;
    DWORD reserved;
    DWORD headerAddress;
    DWORD optionalHeaderEntryCount;
};

struct XexOptionalHeaderEntry
{
    XexOptionHeaderEntryID id;
    DWORD data;
};

struct XexStaticLibraryInfo
{
    std::string name;
    Version version;
};

struct XexRatingBlock
{
    ESRBRating ESRB;
    PEGIRating PEGI;
    PEGIFIRating PEGIFI;
    PEGIPTRating PEGIPT;
    PEGIBBFCRating PEGIBBFC;
};

struct XexExecutionInfo
{
    DWORD mediaID;
    DWORD version;
    DWORD baseVersion;
    DWORD titleID;
    BYTE executionTable;
    BYTE platform;
    BYTE discNumber;
    BYTE discCount;
    DWORD savegameID;
};

struct XexResourceFileEntry
{
    std::string name; 	// max length is 8 characters
    DWORD address;
    DWORD size;
};

struct XexSecurityInfo
{
    DWORD size;
    DWORD imageSize;
    BYTE pirsRsaSignature[0x100];
    DWORD imageInfoSize;
    DWORD imageFlags;
    DWORD loadAddress;
    BYTE sectionHash[0x14];
    DWORD importTableSize;
    BYTE importTableHash[0x14];
    BYTE mediaID[0x10];
    BYTE key[XEX_AES_BLOCK_SIZE];
    DWORD exportTableSize;
    BYTE headerHash[0x14];
    DWORD regions;
    DWORD allowedMediaTypes;
};

enum XexSectionType
{
    XexSectionCode = 1,
    XexSectionData = 2,
    XexSectionReadOnlyData = 3
};

enum XexEncryptionState
{
    RetailEncrypted,
    DevKitEncrypted,
    Decrypted
};

struct XexSectionEntry
{
    XexSectionType type;
    DWORD totalSize;
    BYTE hash[0x14];
};

struct XexCompressionBlock
{
    DWORD size;
    DWORD nullSize;
};

#endif // XEXDEFINITIONS_H
