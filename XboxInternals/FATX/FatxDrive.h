#ifndef FATXDRIVE_H
#define FATXDRIVE_H

#include "../IO/DeviceIO.h"
#include "../IO/FatxIO.h"
#include "../Cryptography/XeKeys.h"
#include "FatxConstants.h"

class XBOXINTERNALSSHARED_EXPORT FatxDrive
{

public:
    FatxDrive(std::string drivePath);
    FatxDrive(std::wstring drivePath);
    ~FatxDrive();

    // get the drives partitions
    std::vector<Partition*> GetPartitions();

    // get a FatxIO for the given entry
    FatxIO GetFatxIO(FatxFileEntry *entry);

    // populate entry's cachedFiles vector (only if it's a directory)
    void GetChildFileEntries(FatxFileEntry *entry);

    // populate entry's clusterChain with its cluster chain
    void ReadClusterChain(FatxFileEntry *entry);

    // convert a cluster to an offset
    static INT64 ClusterToOffset(Partition *part, DWORD cluster);

private:
    // find count amount of free custers
    std::vector<DWORD> getFreeClusters(Partition *part, DWORD count);

    // open up a physical drive
    void loadFatxDrive(std::wstring drivePath);

    // process a partition and load it with calulated information
    void processBootSector(Partition *part);

    // counts the largest amount of consecutive unset bits
    static BYTE cntlzw(DWORD x);

    DeviceIO *io;
    SecurityInfo securityBlob;
    std::vector<Partition*> partitions;
};

#endif // FATXDRIVE_H
