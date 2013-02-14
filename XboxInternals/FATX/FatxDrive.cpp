#include "FatxDrive.h"

FatxDrive::FatxDrive(std::wstring drivePath)
{
    io = new DeviceIO(drivePath);

    // parse the security blob
    io->SetPosition(0x2000);
    securityBlob.serialNumber = io->ReadString(0x14);
    securityBlob.firmwareRevision = io->ReadString(8);
    securityBlob.modelNumber = io->ReadString(0x28);
    io->ReadBytes(securityBlob.msLogoHash, 0x14);
    securityBlob.userAddressableSectors = io->ReadDword();
    io->ReadBytes(securityBlob.rsaSignature, 0x100);

    // TODO: verify the signature

    // seek to the next sector
    io->SetPosition(0x2200);
    securityBlob.msLogoSize = io->ReadDword();
    securityBlob.msLogo = new BYTE[securityBlob.msLogoSize];
    io->ReadBytes(securityBlob.msLogo, securityBlob.msLogoSize);

    Partition *data = new Partition();
    data->address = HddOffsets::Data;
    data->size = io->DriveLength() - data->address;
    data->name = "Data";
    processBootSector(data);
}

void FatxDrive::processBootSector(Partition *part)
{
    UINT64 partitionSize = part->size;

    // seek to the partition
    io->SetPosition(part->address);

    // read the header
    part->magic = io->ReadDword();
    if (part->magic != FATX_MAGIC)
        throw std::string("FATX: Invalid partition magic.\n");

    part->partitionId = io->ReadDword();
    part->sectorsPerCluster = io->ReadDword();
    part->rootDirectoryCluster = io->ReadDword();

    switch(part->sectorsPerCluster)
    {
        case 0x2:
        case 0x4:
        case 0x8:
        case 0x10:
        case 0x20:
        case 0x40:
        case 0x80:
            break;
        default:
            throw std::string("FATX: Found invalid sectors per cluster.\n");
    }

    part->clusterSize = part->sectorsPerCluster << 9;
    BYTE consecutiveZeroes = cntlzw(part->clusterSize);
    int shiftFactor = 0x1F - consecutiveZeroes;

    partitionSize >>= shiftFactor;
    partitionSize++;

    if (partitionSize < FAT_CLUSTER16_RESERVED)
        part->fatEntryShift = 1;
    else
        part->fatEntryShift = 2;

    partitionSize <<= part->fatEntryShift;
    partitionSize += 0x1000;
    partitionSize--;

    UINT64 clusters = part->size;
    clusters -= 0x1000;

    partitionSize &= ~0xFFF;
    partitionSize &= 0xFFFFFFFF;

    if (clusters < partitionSize)
        throw std::string("FATX: Volume too small to hold the FAT.\n");

    clusters -= partitionSize;
    clusters >>= (shiftFactor & 0xFFFFFFFFFFFFFF);
    if (clusters > 0xFFFFFFF)
        throw std::string("FATX: Too many clusters.\n");

    part->clusterCount = clusters;
    part->allocationTableSize = partitionSize;
    part->clusterEntrySize = (clusters < FAT_CLUSTER16_RESERVED) ? FAT16 : FAT32;
    part->clusterStartingAddress = part->address + partitionSize + 0x1000;
}

BYTE FatxDrive::cntlzw(DWORD x)
{
    if (x == 0)
        return 0;

    DWORD t = 0x80000000;
    BYTE largestCount = 0, currentCount = 0;
    BYTE n = 0;

    while (n < 32)
    {
        BOOL bitSet = (x & t) >> (31 - n);
        if (bitSet)
        {
            if (currentCount > largestCount)
                largestCount = currentCount;
            currentCount = 0;
        }
        else
            currentCount++;

        t >>= 1;
        n++;
    }

    if (currentCount > largestCount)
        largestCount = currentCount;

    return largestCount;
}

FatxDrive::~FatxDrive()
{
    delete[] securityBlob.msLogo;

    io->Close();
    delete io;
}
