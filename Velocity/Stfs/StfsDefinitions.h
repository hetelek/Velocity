#ifndef STFSDEFINITIONS_H
#define STFSDEFINITIONS_H

#include "winnames.h"
#include "StfsConstants.h"
#include "FileIO.h"
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

struct VolumeDescriptor
{
    BYTE size;
    BYTE reserved;
    BYTE blockSeperation;
    WORD fileTableBlockCount;
    INT24 fileTableBlockNum;
    BYTE topHashTableHash[0x14];
    DWORD allocatedBlockCount;
    DWORD unallocatedBlockCount;
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

void ReadVolumeDescriptorEx(VolumeDescriptor *descriptor, FileIO *io, DWORD address);

void WriteVolumeDescriptorEx(VolumeDescriptor *descriptor, FileIO *io, DWORD address);

void ReadCertificateEx(Certificate *cert, FileIO *io, DWORD address);

void WriteCertificateEx(Certificate *cert, FileIO *io, DWORD address);

string MagicToString(Magic magic);

string ContentTypeToString(ContentType type);

string ConsoleTypeToString(ConsoleType type);

string ByteSizeToString(int bytes);

string LicenseTypeToString(LicenseType type);

MSTime DWORDToMSTime(DWORD winTime);

DWORD MSTimeToDWORD(MSTime time);

MSTime TimetToMSTime(time_t time);

#endif // STFSDEFINITIONS_H
