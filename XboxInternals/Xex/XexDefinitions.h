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

enum XexOptionHeaderEntryID
{
    ResourceInfo = 0x2FF,					// out
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
    BaseFileDescriptor = 0x3FF,				// out
    CallcapInformation = 0x18102,			// out, not sure how long this is but if the entry exists then the "Image is enabled for callcap"
    FastcapInformation = 0x18200,			// out, not sure how long this is but if the entry exists then the "Image is enabled for fastcap"
    OriginalPEImageName = 0x183FF,			// out
    Xbox360Logo = 0x405FF,					// out, not sure on the format of this, most likely a variation of a bitmap
    RatingInformation = 0x40310
};

enum ModuleFlags
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

enum XexRegionMasks
{
    XexRegionMaskNorthAmerica = 0xFF,
    XexRegionMaskJapan = 0x100,
    XexRegionMaskChina = 0x200,
    XexRegionMaskRestOfAsia = 0xFC00,
    XexRegionMaskAustraliaNewZealand = 0x10000,
    XexRegionMaskRestOfEurope = 0xFE0000,
    XexRegionMaskRestOfWorld = 0xFF000000
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

struct XexHeader
{
    DWORD magic;
    DWORD moduleFlags;
    DWORD dataAddress;
    DWORD reserved;
    DWORD fileHeaderAddress;
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

#endif // XEXDEFINITIONS_H
