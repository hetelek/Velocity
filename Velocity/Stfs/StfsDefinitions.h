#ifndef STFSDEFINITIONS_H
#define STFSDEFINITIONS_H

#include "winnames.h"
#include "StfsConstants.h"
#include <time.h>

#include <QString>

class FileIO;

enum LicenseType
{
    Unused = 0x0000,
    Unrestricted = 0xFFFF,
    ConsoleProfileLicense = 0x0009,
    WindowsProfileLicense = 0x0003,
    ConsoleLicense = 0xF000,
    //These unknowns seem to be media related.
    Unknown1 = 0xE000,
    Unknown2 = 0xD000,
    Unknown3 = 0xC000,
    Unknown4 = 0xB000
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
    QString ownerConsolePartNumber;
    ConsoleType ownerConsoleType;
    ConsoleTypeFlags consoleTypeFlags;
    QString dateGeneration;
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

QString MagicToString(Magic magic);

QString ContentTypeToString(ContentType type);

QString ConsoleTypeToString(ConsoleType type);

QString ByteSizeToString(int bytes);

QString LicenseTypeToString(LicenseType type);

MSTime DWORDToMSTime(DWORD winTime);

DWORD MSTimeToDWORD(MSTime time);

MSTime TimetToMSTime(time_t time);

#endif // STFSDEFINITIONS_H
