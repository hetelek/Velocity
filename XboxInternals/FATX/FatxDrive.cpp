#include "FatxDrive.h"

FatxDrive::FatxDrive(std::string drivePath)
{
    // convert it to a wstring
    std::wstring wsDrivePath;
    wsDrivePath.assign(drivePath.begin(), drivePath.end());

    // load the device
    loadFatxDrive(wsDrivePath);
}

FatxDrive::FatxDrive(std::wstring drivePath)
{
    loadFatxDrive(drivePath);
}

std::vector<Partition*> FatxDrive::GetPartitions()
{
    return partitions;
}

FatxIO FatxDrive::GetFatxIO(FatxFileEntry *entry)
{
    // only if it hasn't been read yet
    if (entry->clusterChain.size() == 0)
        ReadClusterChain(entry);

    return FatxIO(io, entry);
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
    part->lastFreeClusterFound = 1;

    // setup the root
    part->root.startingCluster = part->rootDirectoryCluster;
    part->root.readDirectories = false;
    part->root.name = part->name;
    part->root.partition = part;
    part->root.fileAttributes = FatxDirectory;
    part->root.address = -1;
    part->root.path = "Drive:\\";
}

void FatxDrive::ExtractSecurityBlob(string path)
{
    INT64 originalPos = io->GetPosition();

    // create a file to save to
    FileIO outFile(path, true);

    // seek to the beginning of the security blob
    io->SetPosition(0x2000);

    // allocate memory for the data
    BYTE *buffer = new BYTE[securityBlob.msLogoSize + 0x204];

    // read in the data
    io->ReadBytes(buffer, securityBlob.msLogoSize + 0x204);
    outFile.WriteBytes(buffer, securityBlob.msLogoSize + 0x204);
    outFile.Flush();

    // cleanup
    delete[] buffer;
    outFile.Close();
    io->SetPosition(originalPos);
}

void FatxDrive::CreateFileEntry(FatxFileEntry *parent, FatxFileEntry *newEntry)
{
    if (!(parent->fileAttributes & FatxDirectory))
        throw std::string("FATX: Parent file entry is not a directory.\n");

    GetChildFileEntries(parent);
    for (int i = 0; i < parent->cachedFiles.size(); i++)
        if (parent->cachedFiles.at(i).name == newEntry->name)
            throw std::string("FATX: Entry already exists.\n");

    newEntry->nameLen = newEntry->name.length();

    UINT64 freeEntryAddress = parent->cachedFiles.size() * FATX_ENTRY_SIZE;
    FatxIO parentIO = GetFatxIO(parent);

    // check to make sure this address is the correct one
    if (freeEntryAddress != 0)
    {
        parentIO.SetPosition(freeEntryAddress - FATX_ENTRY_SIZE);

        BYTE prevNameLength = parentIO.ReadByte();
        if (prevNameLength == 0xFF || prevNameLength == 0)
            throw std::string("FATX: Could not calculate correct entry address.\n");
    }

    parentIO.AllocateMemory(FATX_ENTRY_SIZE);

    newEntry->startingCluster = FAT_CLUSTER_LAST;

    DWORD fileSize = newEntry->fileSize;
    newEntry->fileSize = 0;
    newEntry->partition = parent->partition;

    FatxIO childIO = GetFatxIO(newEntry);

    parentIO.SetPosition(freeEntryAddress);
    newEntry->address = parentIO.GetDrivePosition();

    childIO.AllocateMemory(fileSize);

    parent->cachedFiles.push_back(*newEntry);
}

void FatxDrive::GetChildFileEntries(FatxFileEntry *entry)
{
    // if all entries have been read, skip this
    if (entry->readDirectories || !(entry->fileAttributes & FatxDirectory))
        return;

    // if the cluster chain has already been read, skip this
    if (entry->clusterChain.size() == 0)
        ReadClusterChain(entry);

    // find out how many entries are in a single cluster
    DWORD entriesInCluster = entry->partition->clusterSize / FATX_ENTRY_SIZE;

    bool doneForGood = false;

    // read all entries
    for (int i = 0; i < entry->clusterChain.size(); i++)
    {
        UINT64 posCur = FatxIO::ClusterToOffset(entry->partition, entry->clusterChain.at(i));

        // go to the cluster offset
        io->SetPosition(posCur);

        for (int x = 0; x < entriesInCluster; x++)
        {
            // read the name length
            FatxFileEntry newEntry;
            newEntry.nameLen = io->ReadByte();

            // check if there are no more entries
            if (newEntry.nameLen == 0xFF || newEntry.nameLen == 0)
            {
                doneForGood = true;
                break;
            }

            // calcualte the address
            newEntry.address = posCur + (x * 0x40);

            // read the attributes
            newEntry.fileAttributes = io->ReadByte();

            // read the name (0xFF is the null terminator)
            bool subtract = true;
            if (newEntry.nameLen == FATX_ENTRY_DELETED)
                newEntry.name = io->ReadString(-1, 0xFF);
            else
            {
                newEntry.name = io->ReadString(newEntry.nameLen);
                subtract = false;
            }

            // seek past the name
            io->SetPosition(io->GetPosition() + (FATX_ENTRY_MAX_NAME_LENGTH - newEntry.name.length() - subtract));

            // read the rest of the entry information
            newEntry.startingCluster = io->ReadDword();
            newEntry.fileSize = io->ReadDword();
            newEntry.creationDate = io->ReadDword();
            newEntry.lastWriteDate = io->ReadDword();
            newEntry.lastAccessDate = io->ReadDword();
            newEntry.partition = entry->partition;
            newEntry.readDirectories = false;
            newEntry.path = entry->path + entry->name + "\\";

            // add it to the file cache
            entry->cachedFiles.push_back(newEntry);
        }
        if (doneForGood)
            break;
    }

    //entry->fileSize = (entry->cachedFiles.size() * FATX_ENTRY_SIZE);
    entry->readDirectories = true;
}

void FatxDrive::ReadClusterChain(FatxFileEntry *entry)
{
    // clear the current chain
    entry->clusterChain.clear();

    // calculate values
    bool clusterSizeIs2 = (entry->partition->clusterEntrySize == FAT16);
    DWORD lastCluster =  (clusterSizeIs2) ? FAT_CLUSTER16_LAST : FAT_CLUSTER_LAST;
    DWORD availableCluster = (clusterSizeIs2) ? FAT_CLUSTER16_AVAILABLE : FAT_CLUSTER_AVAILABLE;
    INT64 clusterMapAddress = entry->partition->address + 0x1000;

    // start with the starting cluster
    DWORD previousCluster = entry->startingCluster;

    while (previousCluster != lastCluster && previousCluster != availableCluster)
    {
        // add it to the cluster chain
        entry->clusterChain.push_back(previousCluster);

        // seek to the next cluster
        io->SetPosition(clusterMapAddress + (previousCluster * entry->partition->clusterEntrySize));

        // read the cluster
        if (clusterSizeIs2)
            previousCluster = io->ReadWord();
        else
            previousCluster = io->ReadDword();
    }
}

void FatxDrive::Close()
{
    io->Close();
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

void FatxDrive::loadFatxDrive(std::wstring drivePath)
{
    // open the device io
    io = new DeviceIO(drivePath);

    // parse the security blob
    io->SetPosition(HddOffsets::SecuritySector);
    securityBlob.serialNumber = io->ReadString(0x14);
    securityBlob.firmwareRevision = io->ReadString(8);
    securityBlob.modelNumber = io->ReadString(0x28);
    io->ReadBytes(securityBlob.msLogoHash, 0x14);
    securityBlob.userAddressableSectors = io->ReadDword();
    io->ReadBytes(securityBlob.rsaSignature, 0x100);
    securityBlob.validSignature = false;

    // TODO: verify the signature

    // seek to the next sector
    io->SetPosition(HddOffsets::SecuritySector + 0x200);
    securityBlob.msLogoSize = io->ReadDword();
    securityBlob.msLogo = new BYTE[securityBlob.msLogoSize];
    io->ReadBytes(securityBlob.msLogo, securityBlob.msLogoSize);

    // if it's a dev drive then we have to read the partition table
    io->SetPosition(0);
    if (io->ReadDword() == FATX_DEVKIT_DRIVE)
    {
        // read the id?
        io->ReadDword();

        Partition *content = new Partition;
        content->address = (UINT64)io->ReadDword() * 0x200;
        content->size = (UINT64)io->ReadDword() * 0x200;
        content->name = "Content";

        Partition *dashboard = new Partition;
        dashboard->address = (UINT64)io->ReadDword() * 0x200;
        dashboard->size = (UINT64)io->ReadDword() * 0x200;
        dashboard->name = "Xbox 360 Dashboard";

        partitions.push_back(content);
        partitions.push_back(dashboard);
    }
    else
    {
        // system extended partition initialization
        Partition *systemExtended = new Partition;
        systemExtended->address = HddOffsets::SystemExtended;
        systemExtended->size = HddSizes::SystemExtended;
        systemExtended->name = "System Extended";

        // system auxiliary partition initialization
        Partition *systemAuxiliary = new Partition;
        systemAuxiliary->address = HddOffsets::SystemAuxiliary;
        systemAuxiliary->size = HddSizes::SystemAuxiliary;
        systemAuxiliary->name = "System Auxiliary";

        // system partition initialization
        Partition *systemPartition = new Partition;
        systemPartition->address = HddOffsets::SystemPartition;
        systemPartition->size = HddSizes::SystemPartition;
        systemPartition->name = "System Partition";

        // content partition initialization
        Partition *content = new Partition;
        content->address = HddOffsets::Data;
        content->size = io->DriveLength() - content->address;
        content->name = "Content";

        // add the partitions to the vector
        this->partitions.push_back(systemExtended);
        this->partitions.push_back(systemAuxiliary);
        this->partitions.push_back(systemPartition);
        this->partitions.push_back(content);
    }

    // process all bootsectors
    for (int i = 0; i < this->partitions.size(); i++)
        processBootSector(this->partitions.at(i));
}
