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
    std::vector<Partition*> GetPartitions();
    FatxIO GetFatxIO(FatxFileEntry *entry);

    void GetChildFileEntries(FatxFileEntry *entry);

    static INT64 ClusterToOffset(Partition *part, DWORD cluster);
    ~FatxDrive();

    SecurityInfo securityBlob;

private:
    void readClusterChain(FatxFileEntry *entry);
    void loadFatxDrive(std::wstring drivePath);
    void processBootSector(Partition *part);
    static BYTE cntlzw(DWORD x);

    DeviceIO *io;
    std::vector<Partition*> partitions;
};

#endif // FATXDRIVE_H
