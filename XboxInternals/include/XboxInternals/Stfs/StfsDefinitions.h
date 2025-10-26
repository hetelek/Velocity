#ifndef STFSDEFINITIONS_H
#define STFSDEFINITIONS_H

#include <XboxInternals/TypeDefinitions.h>
#include <XboxInternals/Stfs/StfsConstants.h>
#include <XboxInternals/IO/FileIO.h>
#include <iostream>
#include <sstream>
#include <time.h>

using std::string;

enum LicenseType
{
    Unused = 0x0000,
    Unrestricted = 0xFFFF,
    ConsoleProfileLicense = 0x0009,
    WindowsProfileLicense = 0x0003,
    ConsoleLicense = 0xF000,
    MediaFlags = 0xE000,
    KeyVaultPrivileges = 0xD000,
    HyperVisorFlags = 0xC000,
    UserPrivileges = 0xB000
};

struct LicenseEntry
{
    LicenseType type;
    UINT64 data;
    DWORD bits;
    DWORD flags;
};

struct StfsVolumeDescriptor
{
    BYTE size;
    BYTE reserved;
    BYTE blockSeparation;
    WORD fileTableBlockCount;
    INT24 fileTableBlockNum;
    BYTE topHashTableHash[0x14];
    DWORD allocatedBlockCount;
    DWORD unallocatedBlockCount;
};

struct SvodVolumeDescriptor
{
    BYTE size;
    BYTE blockCacheElementCount;
    BYTE workerThreadProcessor;
    BYTE workerThreadPriority;
    BYTE rootHash[0x14];
    BYTE flags;
    INT24 dataBlockCount;
    INT24 dataBlockOffset;
    BYTE reserved[5];
};

struct Certificate
{
    WORD publicKeyCertificateSize;
    BYTE ownerConsoleID[5];
    string ownerConsolePartNumber;
    ConsoleType ownerConsoleType;
    ConsoleTypeFlags consoleTypeFlags;
    string dateGeneration;
    DWORD publicExponent;
    BYTE publicModulus[0x80];
    BYTE certificateSignature[0x100];
    BYTE signature[0x80];
};

struct MSTime
{
    WORD year;
    BYTE month;
    BYTE monthDay;

    BYTE hours;
    BYTE minutes;
    BYTE seconds;
};

void ReadStfsVolumeDescriptorEx(StfsVolumeDescriptor *descriptor, BaseIO *io, DWORD address);

void ReadSvodVolumeDescriptorEx(SvodVolumeDescriptor *descriptor, BaseIO *io);

void WriteStfsVolumeDescriptorEx(StfsVolumeDescriptor *descriptor, BaseIO *io, DWORD address);

void WriteSvodVolumeDescriptorEx(SvodVolumeDescriptor *descriptor, BaseIO *io);

void ReadCertificateEx(Certificate *cert, BaseIO *io, DWORD address);

void WriteCertificateEx(Certificate *cert, BaseIO *io, DWORD address);

XBOXINTERNALS_EXPORT string MagicToString(Magic magic);

XBOXINTERNALS_EXPORT string ContentTypeToString(ContentType type);

XBOXINTERNALS_EXPORT string ConsoleTypeToString(ConsoleType type);

XBOXINTERNALS_EXPORT string ByteSizeToString(UINT64 bytes);

XBOXINTERNALS_EXPORT string LicenseTypeToString(LicenseType type);

XBOXINTERNALS_EXPORT MSTime DWORDToMSTime(DWORD winTime);

XBOXINTERNALS_EXPORT DWORD MSTimeToDWORD(MSTime time);

XBOXINTERNALS_EXPORT MSTime TimetToMSTime(time_t time);

#endif // STFSDEFINITIONS_H


