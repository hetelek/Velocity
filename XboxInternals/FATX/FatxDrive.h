#ifndef FATXDRIVE_H
#define FATXDRIVE_H

#include <iostream>

#include "../IO/DeviceIO.h"
#include "../Cryptography/XeKeys.h"
#include "FatxConstants.h"

struct SecurityInfo
{
    std::string serialNumber;
    std::string firmwareRevision;
    std::string modelNumber;
    BYTE msLogoHash[0x14];
    DWORD userAddressableSectors;
    BYTE rsaSignature[0x100];
    bool validSignature;

    DWORD msLogoSize;
    BYTE *msLogo;
};

struct Partition
{
    std::string name;
    INT64 address;
    UINT64 size;

    // header
    DWORD magic;
    DWORD partitionId;
    DWORD sectorsPerCluster;
    DWORD rootDirectoryCluster;

    BYTE clusterEntrySize;
    DWORD clusterCount;
    DWORD clusterSize;
    UINT64 clusterStartingAddress;

    DWORD fatEntryShift;
    UINT64 allocationTableSize;
};

class XBOXINTERNALSSHARED_EXPORT FatxDrive
{

public:
    FatxDrive(std::wstring drivePath);
    ~FatxDrive();

    SecurityInfo securityBlob;

private:
    void processBootSector(Partition *part);
    static BYTE cntlzw(DWORD x);
    DeviceIO *io;
};

#endif // FATXDRIVE_H
