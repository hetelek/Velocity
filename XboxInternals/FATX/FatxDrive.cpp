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
    {
        part->address = 0;
        return;
    }

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
    part->freeMemory = 0;

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

    // set the address to null
    newEntry->address = 0;

    // get the child entries
    GetChildFileEntries(parent);

    for (int i = 0; i < parent->cachedFiles.size(); i++)
        if (parent->cachedFiles.at(i).name == newEntry->name)
        {
            // if it's deleted, it's okay!
            if (parent->cachedFiles.at(i).nameLen != FATX_ENTRY_DELETED)
                throw std::string("FATX: Entry already exists.\n");

            newEntry->address = parent->cachedFiles.at(i).address;
        }

    // set the name length
    newEntry->nameLen = newEntry->name.length();

    if (newEntry->address == 0)
    {
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

        if (parentIO.AllocateMemory(FATX_ENTRY_SIZE) != 0)
        {
            parentIO.SetPosition(parent->fileSize);
            parentIO.Write((BYTE)0xFF);
        }

        parentIO.SetPosition(freeEntryAddress);
        newEntry->address = parentIO.GetDrivePosition();
    }

    DWORD fileSize = newEntry->fileSize;
    newEntry->fileSize = 0;
    newEntry->partition = parent->partition;

    FatxIO childIO = GetFatxIO(newEntry);
    newEntry->clusterChain.clear();
    childIO.AllocateMemory(fileSize);

    parent->cachedFiles.push_back(*newEntry);
}

void FatxDrive::DeleteFile(FatxFileEntry *entry)
{
    // read the data
    GetChildFileEntries(entry);
    ReadClusterChain(entry);

    for (int i = 0; i < entry->cachedFiles.size(); i++)
        DeleteFile(&entry->cachedFiles.at(i));

    // set all the clusters to available
    entry->clusterChain.push_back(entry->startingCluster);
    FatxIO::SetAllClusters(io, entry->partition, entry->clusterChain, FAT_CLUSTER_AVAILABLE);

    // update the entry
    entry->clusterChain.clear();
    entry->nameLen = FATX_ENTRY_DELETED;

    // update the entry file name lenght to deleted
    io->SetPosition(entry->address);
    io->Write((BYTE)FATX_ENTRY_DELETED);
}

void FatxDrive::InjectFile(FatxFileEntry *parent, std::string name, std::string filePath, void (*progress)(void *, DWORD, DWORD), void *arg)
{
    FatxFileEntry entry;
    entry.name = name;

    // seek to the end of the file
    FileIO inFile(filePath);
    inFile.SetPosition(0, ios_base::end);

    // get the file size
    UINT64 fileSize = inFile.GetPosition();
    entry.fileSize = fileSize;

    // set the times
    DWORD currentTime = MSTimeToDWORD(TimetToMSTime(time(NULL)));
    entry.creationDate = currentTime;
    entry.lastAccessDate = currentTime;
    entry.lastWriteDate = currentTime;

    // set other stuff
    entry.fileAttributes = 0;

    // create the entry
    CreateFileEntry(parent, &entry);

    ////////////////////////////
    // START WRITING THE DATA //
    ////////////////////////////

    // write the data
    FatxIO entryIO = GetFatxIO(&entry);

    // seek to the beginning of the files
    entryIO.SetPosition(0);
    inFile.SetPosition(0);

    // calculate stuff
    DWORD bufferSize = (entry.fileSize / 0x10);
    if (bufferSize < 0x10000)
        bufferSize = 0x10000;
    else if (bufferSize > 0x100000)
        bufferSize = 0x100000;

    std::vector<Range> writeRanges;
    BYTE *buffer = new BYTE[bufferSize];
    INT64 pos;

    // generate the read ranges
    for (DWORD i = 0; i < entry.clusterChain.size() - 1; i++)
    {
        // calculate cluster's address
        pos = FatxIO::ClusterToOffset(entry.partition, entry.clusterChain.at(i));

        Range range = { pos, 0 };
        do
        {
            range.len += entry.partition->clusterSize;
        }
        while ((entry.clusterChain.at(i) + 1) == entry.clusterChain.at(++i) && i < (entry.clusterChain.size() - 2) && (range.len + entry.partition->clusterSize) <= bufferSize);
        i--;

        writeRanges.push_back(range);
    }

    DWORD finalClusterSize = entry.fileSize % entry.partition->clusterSize;
    INT64 finalClusterOffset = FatxIO::ClusterToOffset(entry.partition, entry.clusterChain.at(entry.clusterChain.size() - 1));
    Range lastRange = { finalClusterOffset , (finalClusterSize == 0) ? entry.partition->clusterSize : finalClusterSize };
    writeRanges.push_back(lastRange);

    DWORD modulus = writeRanges.size() / 100;
    if (modulus == 0)
        modulus = 1;
    else if (modulus > 3)
        modulus = 3;

    // read all the data in
    for (DWORD i = 0; i < writeRanges.size(); i++)
    {
        // seek to the beginning of the range
        io->SetPosition(writeRanges.at(i).start);

        // get the range from the device
        inFile.ReadBytes(buffer, writeRanges.at(i).len);
        io->WriteBytes(buffer, writeRanges.at(i).len);

        // update progress if needed
        if (progress && i % modulus == 0)
            progress(arg, i + 1, writeRanges.size());
    }

    // make sure it hits the end
    progress(arg, writeRanges.size(), writeRanges.size());

    inFile.Close();

    delete[] buffer;
}

void FatxDrive::GetFileEntryMagic(FatxFileEntry *entry)
{
    if (entry->fileSize < 4 || entry->magic != 0)
        return;

    io->SetPosition(FatxIO::ClusterToOffset(entry->partition, entry->startingCluster));
    entry->magic = io->ReadDword();
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
                newEntry.name = io->ReadString(-1, 0xFF, true, FATX_ENTRY_MAX_NAME_LENGTH);
            else
            {
                newEntry.name = io->ReadString(newEntry.nameLen);
                subtract = false;
            }

            // seek past the name
            io->SetPosition(io->GetPosition() + (FATX_ENTRY_MAX_NAME_LENGTH - newEntry.name.length()) - subtract);

            // read the rest of the entry information
            newEntry.startingCluster = io->ReadDword();
            newEntry.fileSize = io->ReadDword();
            newEntry.creationDate = io->ReadDword();
            newEntry.lastWriteDate = io->ReadDword();
            newEntry.lastAccessDate = io->ReadDword();
            newEntry.partition = entry->partition;
            newEntry.readDirectories = false;
            newEntry.path = entry->path + entry->name + "\\";
            newEntry.magic = 0;

            // add it to the file cache
            entry->cachedFiles.push_back(newEntry);
        }
        if (doneForGood)
            break;
    }

    entry->fileSize = (entry->cachedFiles.size() * FATX_ENTRY_SIZE);
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

void FatxDrive::loadProfiles()
{
    // the general path for profiles is Drive:\Content\Content\OFFLINE_XUID\FFFE07D1\00010000\OFFLINE_XUID

    FatxFileEntry *contentRoot = NULL;

    // get the content partition's root
    for (DWORD i = 0; i < partitions.size(); i++)
        if (partitions.at(i)->name == "Content")
            contentRoot = &partitions.at(i)->root;

    // if the content partition doesn't exist, then return
    if (contentRoot == NULL)
        return;

    FatxFileEntry *contentFolder = NULL;

    // get the content folder in the content partition
    GetChildFileEntries(contentRoot);
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
    io->SetEndian(LittleEndian);
    securityBlob.userAddressableSectors = io->ReadDword();
    io->SetEndian(BigEndian);
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
    {
        processBootSector(this->partitions.at(i));

        // if there's invalid magic
        if (this->partitions.at(i)->address == 0)
        {
            delete partitions.at(i);
            partitions.erase(partitions.begin() + i);
        }
    }
}

UINT64 FatxDrive::GetFreeMemory(Partition *part)
{
    if (part->freeMemory != 0)
        return;

    // allocate memory for a buffer to minimize the amount of reads
    BYTE *buffer = new BYTE[0x50000];

    // seek to the chainmap
    io->SetPosition(part->address + 0x1000);

    // check if it's FAT16
    bool clusterSizeIs2 = (part->clusterEntrySize == FAT16);

    DWORD bytesLeft = part->clusterCount * 4;
    UINT64 freeClusters = 0;
    DWORD readSize;
    while (bytesLeft > 0)
    {
        // calculate the read size
        readSize = (bytesLeft > 0x50000) ? 0x50000 : bytesLeft;
        bytesLeft -= readSize;

        // read in the segment
        io->ReadBytes(buffer, readSize);
        MemoryIO memory(buffer, 0x50000);
        memory.SetPosition(0);

        // iterate through all of the clusters
        if (clusterSizeIs2)
        {
            for (DWORD i = 0; i < (readSize / 2); i++)
                freeClusters += (memory.ReadWord() == FAT_CLUSTER16_AVAILABLE);
        }
        else
        {
            for (DWORD i = 0; i < (readSize / 4); i++)
                freeClusters += (memory.ReadDword() == FAT_CLUSTER_AVAILABLE);
        }
    }

    // calculate the amount of free memory
    part->freeMemory = freeClusters * part->clusterSize;

    // cleanup
    delete[] buffer;

    return part->freeMemory;
}
