#ifndef FATXDRIVE_H
#define FATXDRIVE_H

#include <iostream>
#include <vector>

#include "../IO/DeviceIO.h"
#include "../Cryptography/XeKeys.h"
#include "FatxConstants.h"

struct Partition;

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

struct FatxFileEntry
{
    Partition *partition;

    BYTE nameLen;
    BYTE fileAttributes;

    std::string name;
    DWORD startingCluster;
    DWORD fileSize;

    // times
    DWORD creationDate;
    DWORD lastWriteDate;
    DWORD lastAccessDate;

    bool readDirectories;
    std::vector<FatxFileEntry> cachedFiles;
    std::vector<DWORD> clusterChain;
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

    // partition entries
    FatxFileEntry root;

    // calculated information
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
    FatxDrive(std::string drivePath);
    FatxDrive(std::wstring drivePath);
    std::vector<Partition*> GetPartitions();
    void GetChildFileEntries(FatxFileEntry *entry);
    ~FatxDrive();

    SecurityInfo securityBlob;

private:
    void loadFatxDrive(std::wstring drivePath);
    void processBootSector(Partition *part);
    void readClusterChain(FatxFileEntry *entry);
    INT64 clusterToOffset(Partition *part, DWORD cluster);
    static BYTE cntlzw(DWORD x);

    DeviceIO *io;
    std::vector<Partition*> partitions;
};

#endif // FATXDRIVE_H
