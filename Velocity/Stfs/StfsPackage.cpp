#include "StfsPackage.h"
#include "StfsMetaData.h"

#include <stdio.h>

StfsPackage::StfsPackage(string packagePath, DWORD flags) : flags(flags)
{
    io = new FileIO(packagePath, (bool)(flags & StfsPackageCreate));

    // if we need to create a file, then do it yo
    if (flags & StfsPackageCreate)
    {
        DWORD headerSize = (flags & StfsPackagePEC) ? ((flags & StfsPackageFemale) ? 0x2000 : 0x1000) : ((flags & StfsPackageFemale) ? 0xB000 : 0xA000);
        BYTE zeroBuffer[0x1000] = {0};

        // write all null bytes for the header
        for (DWORD i = 0; i < ((headerSize >> 0xC) + ((flags & StfsPackageFemale) ? 1 : 2) + 1); i++)
            io->write(zeroBuffer, 0x1000);

        // if it's female, then we need to write it to the volume descriptor
        io->setPosition((flags & StfsPackagePEC) ? 0x246 : 0x37B);
        io->write((flags & StfsPackageFemale) >> 2);
    }

    Parse();
}

void StfsPackage::Parse()
{
    if (flags & StfsPackageCreate)
        metaData = new StfsMetaData(io, (flags & StfsPackagePEC) | MetadataSkipRead | MetadataDontFreeThumbnails);
    else
        metaData = new StfsMetaData(io, (flags & StfsPackagePEC));

    // if the pacakge was created, then give all the metadata a default value
    if (flags & StfsPackageCreate)
    {
        metaData->magic = CON;
        metaData->certificate.publicKeyCertificateSize = 0x1A8;
        metaData->certificate.ownerConsoleType = Retail;
        metaData->certificate.consoleTypeFlags = (ConsoleTypeFlags)0;

        memset(metaData->licenseData, 0, sizeof(LicenseEntry) * 0x10);
        metaData->licenseData[0].type = Unrestricted;
        metaData->licenseData[0].data = 0xFFFFFFFFFFFF;

        DWORD headerSize;
        if (flags & StfsPackagePEC)
            headerSize = (flags & StfsPackageFemale) ? 0x2000 : 0x1000;
        else
            headerSize = (flags & StfsPackageFemale) ? 0xAD0E : 0x971A;

        metaData->headerSize = headerSize;
        metaData->contentType = (ContentType)0;
        metaData->metaDataVersion = 2;
        metaData->contentSize = 0;
        metaData->mediaID = 0;
        metaData->version = 0;
        metaData->baseVersion = 0;
        metaData->titleID = 0;
        metaData->platform = 0;
        metaData->executableType = 0;
        metaData->discNumber = 0;
        metaData->discInSet = 0;
        metaData->savegameID = 0;
        memset(metaData->consoleID, 0, 5);
        memset(metaData->profileID, 0, 8);

        // volume descriptor
        metaData->volumeDescriptor.size = 0x24;
        metaData->volumeDescriptor.blockSeperation = ((flags & StfsPackageFemale) >> 2);
        metaData->volumeDescriptor.fileTableBlockCount = 1;
        metaData->volumeDescriptor.fileTableBlockNum = 0;
        metaData->volumeDescriptor.allocatedBlockCount = 1;
        metaData->volumeDescriptor.unallocatedBlockCount = 0;

        metaData->dataFileCount = 0;
        metaData->dataFileCombinedSize = 0;
        metaData->seasonNumber = 0;
        metaData->episodeNumber = 0;
        memset(metaData->seasonID, 0, 0x10);
        memset(metaData->seriesID, 0, 0x10);
        memset(metaData->deviceID, 0, 0x14);
        metaData->displayName = L"";
        metaData->displayDescription = L"";
        metaData->publisherName = L"";
        metaData->titleName = L"";
        metaData->transferFlags = 0;
        metaData->thumbnailImage = 0;
        metaData->thumbnailImageSize = 0;
        metaData->titleThumbnailImage = 0;
        metaData->titleThumbnailImageSize = 0;

        metaData->WriteMetaData();

        // set the first block to allocated
        io->setPosition(((headerSize + 0xFFF) & 0xFFFFF000) + 0x14);
        io->write((DWORD)0x80FFFFFF);
    }

    packageSex = (Sex)((~metaData->volumeDescriptor.blockSeperation) & 1);

    if (packageSex == StfsFemale)
    {
        blockStep[0] = 0xAB;
        blockStep[1] = 0x718F;
    }
    else
    {
        blockStep[0] = 0xAC;
        blockStep[1] = 0x723A;
    }

    // address of the first hash table in the package, comes right after the header
    firstHashTableAddress = (metaData->headerSize + 0x0FFF) & 0xFFFFF000;

    // calculate the number of tables per level
    tablesPerLevel[0] = (metaData->volumeDescriptor.allocatedBlockCount / 0xAA) + ((metaData->volumeDescriptor.allocatedBlockCount % 0xAA != 0) ? 1 : 0);
    tablesPerLevel[1] = (tablesPerLevel[0] / 0xAA) + ((tablesPerLevel[0] % 0xAA != 0 && metaData->volumeDescriptor.allocatedBlockCount > 0xAA) ? 1 : 0);
    tablesPerLevel[2] = (tablesPerLevel[1] / 0xAA) + ((tablesPerLevel[1] % 0xAA != 0 && metaData->volumeDescriptor.allocatedBlockCount > 0x70E4) ? 1 : 0);

    // calculate the level of the top table
    topLevel = CalcualateTopLevel();

    // read in the top hash table
    topTable.trueBlockNumber = ComputeLevelNBackingHashBlockNumber(0, topLevel);
    topTable.level = topLevel;

    DWORD baseAddress = (topTable.trueBlockNumber << 0xC) + firstHashTableAddress;
    topTable.addressInFile = baseAddress + ((metaData->volumeDescriptor.blockSeperation & 2) << 0xB);
    io->setPosition(topTable.addressInFile);

    DWORD dataBlocksPerHashTreeLevel[3] = { 1, 0xAA, 0x70E4 };

    // load the information
    topTable.entryCount = metaData->volumeDescriptor.allocatedBlockCount / dataBlocksPerHashTreeLevel[topLevel];
    if (metaData->volumeDescriptor.allocatedBlockCount > 0x70E4 && (metaData->volumeDescriptor.allocatedBlockCount % 0x70E4 != 0))
        topTable.entryCount++;
    else if (metaData->volumeDescriptor.allocatedBlockCount > 0xAA && (metaData->volumeDescriptor.allocatedBlockCount % 0xAA != 0))
        topTable.entryCount++;

    for (DWORD i = 0; i < topTable.entryCount; i++)
    {
        io->readBytes(topTable.entries[i].blockHash, 0x14);
        topTable.entries[i].status = io->readByte();
        topTable.entries[i].nextBlock = io->readInt24();
    }

    // set default values for the root of the file listing
    FileEntry fe;
    fe.pathIndicator = 0xFFFF;
    fe.name = "Root";
    fe.entryIndex = 0xFFFF;
    fileListing.folder = fe;

    ReadFileListing();
}

Level StfsPackage::CalcualateTopLevel()
{
    if (metaData->volumeDescriptor.allocatedBlockCount <= 0xAA)
        return Zero;
    else if (metaData->volumeDescriptor.allocatedBlockCount <= 0x70E4)
        return One;
    else if (metaData->volumeDescriptor.allocatedBlockCount <= 0x4AF768)
        return Two;
    else
        throw string("STFS: Invalid number of allocated blocks.\n");
}

DWORD StfsPackage::ComputeBackingDataBlockNumber(DWORD blockNum)
{
    DWORD toReturn = (((blockNum + 0xAA) / 0xAA) << (BYTE)packageSex) + blockNum;
    if (blockNum < 0xAA)
        return toReturn;
    else if (blockNum < 0x70E4)
        return toReturn + (((blockNum + 0x70E4) / 0x70E4) << (BYTE)packageSex);
    else
        return (1 << (BYTE)packageSex) + (toReturn + (((blockNum + 0x70E4) / 0x70E4) << (BYTE)packageSex));
}

DWORD StfsPackage::BlockToAddress(DWORD blockNum)
{
    // check for invalid block number
    if(blockNum >= INT24_MAX)
        throw string("STFS: Block number must be less than 0xFFFFFF.\n");
    return (ComputeBackingDataBlockNumber(blockNum) << 0x0C) + firstHashTableAddress;
}

bool StfsPackage::IsPEC()
{
    return flags & StfsPackagePEC;
}

DWORD StfsPackage::ComputeLevelNBackingHashBlockNumber(DWORD blockNum, Level level)
{
    switch (level)
    {
        case Zero:
            return ComputeLevel0BackingHashBlockNumber(blockNum);

        case One:
            return ComputeLevel1BackingHashBlockNumber(blockNum);

        case Two:
            return ComputeLevel2BackingHashBlockNumber(blockNum);

        default:
            throw string("STFS: Invalid level.\n");
    }
}

DWORD StfsPackage::ComputeLevel0BackingHashBlockNumber(DWORD blockNum)
{
    if (blockNum < 0xAA)
        return 0;

    DWORD num = (blockNum / 0xAA) * blockStep[0];
    num += ((blockNum / 0x70E4) + 1) << ((BYTE)packageSex);

    if (blockNum / 0x70E4 == 0)
        return num;

    return num + (1 << (BYTE)packageSex);
}

DWORD StfsPackage::ComputeLevel1BackingHashBlockNumber(DWORD blockNum)
{
    if (blockNum < 0x70E4)
        return blockStep[0];
    return (1 << (BYTE)packageSex) + (blockNum / 0x70E4) * blockStep[1];
}

DWORD StfsPackage::ComputeLevel2BackingHashBlockNumber(DWORD /*blockNum*/)
{
    return blockStep[1];
}

DWORD StfsPackage::GetHashAddressOfBlock(DWORD blockNum)
{
    if (blockNum >= metaData->volumeDescriptor.allocatedBlockCount)
         throw string("STFS: Reference to illegal block number.\n");

    DWORD hashAddr = (ComputeLevel0BackingHashBlockNumber(blockNum) << 0xC) + firstHashTableAddress;
    hashAddr += (blockNum % 0xAA) * 0x18;

    switch (topLevel)
    {
        case 0:
            hashAddr += ((metaData->volumeDescriptor.blockSeperation & 2) << 0xB);
            break;
        case 1:
            hashAddr += ((topTable.entries[blockNum / 0xAA].status & 0x40) << 6);
            break;
        case 2:
            DWORD level1Off = ((topTable.entries[blockNum / 0x70E4].status & 0x40) << 6);
            DWORD pos = ((ComputeLevel1BackingHashBlockNumber(blockNum) << 0xC) + firstHashTableAddress + level1Off) +( (blockNum % 0xAA) * 0x18);
            io->setPosition(pos + 0x14);
            hashAddr += ((io->readByte() & 0x40) << 6);
            break;
    }
    return hashAddr;
}

HashEntry StfsPackage::GetBlockHashEntry(DWORD blockNum)
{
    if (blockNum >= metaData->volumeDescriptor.allocatedBlockCount)
        throw string("STFS: Reference to illegal block number.\n");

    // go to the position of the hash address
    io->setPosition(GetHashAddressOfBlock(blockNum));

    // read the hash entry
    HashEntry he;
    io->readBytes(he.blockHash, 0x14);
    he.status = io->readByte();
    he.nextBlock = io->readInt24();

    return he;
}

void StfsPackage::ExtractBlock(DWORD blockNum, BYTE *data, DWORD length)
{
    if (blockNum >= metaData->volumeDescriptor.allocatedBlockCount)
         throw string("STFS: Reference to illegal block number.\n");

    // check for an invalid block length
    if (length > 0x1000)
        throw string("STFS: length cannot be greater 0x1000.\n");

    // go to the block's position
    io->setPosition(BlockToAddress(blockNum));

    // read the data, and return
    io->readBytes(data, length);
}

void StfsPackage::ReadFileListing()
{
    fileListing.fileEntries.clear();
    fileListing.folderEntries.clear();

    // setup the entry for the block chain
    FileEntry entry;
    entry.startingBlockNum = metaData->volumeDescriptor.fileTableBlockNum;
    entry.fileSize = (metaData->volumeDescriptor.fileTableBlockCount * 0x1000);

    // generate a block chain for the full file listing
    DWORD block = entry.startingBlockNum;

    FileListing fl;
    DWORD currentAddr;
    for(DWORD x = 0; x < metaData->volumeDescriptor.fileTableBlockCount; x++)
    {
        currentAddr = BlockToAddress(block);
        io->setPosition(currentAddr);

        for(DWORD i = 0; i < 0x40; i++)
        {
            FileEntry fe;

            // set the current position
            fe.fileEntryAddress = currentAddr + (i * 0x40);

            // calculate the entry index (in the file listing)
            fe.entryIndex = (x * 0x40) + i;

            // read the name, if the length is 0 then break
            fe.name = io->readString(0x28);

            // read the name length
            fe.nameLen = io->readByte();
            if ((fe.nameLen & 0x3F) == 0)
            {
                io->setPosition(currentAddr + ((i + 1) * 0x40));
                continue;
            }
            else if (fe.name.length() == 0)
                break;

            // check for a mismatch in the total allocated blocks for the file
            fe.blocksForFile = io->readInt24(LittleEndian);
            io->setPosition(3, ios_base::cur);

            // read more information
            fe.startingBlockNum = io->readInt24(LittleEndian);
            fe.pathIndicator = io->readWord();
            fe.fileSize = io->readDword();
            fe.createdTimeStamp = io->readDword();
            fe.accessTimeStamp = io->readDword();

            // get the flags
            fe.flags = fe.nameLen >> 6;

            // bits 6 and 7 are flags, clear them
            fe.nameLen &= 0x3F;

            fl.fileEntries.push_back(fe);
        }

        block = GetBlockHashEntry(block).nextBlock;
    }

    // sort the file listing
    AddToListing(&fl, &fileListing);
    writtenToFile = fileListing;
}

FileListing StfsPackage::GetFileListing(bool forceUpdate)
{
    // update the file listing from file if requested
    if(forceUpdate)
        ReadFileListing();

    return fileListing;
}

DWORD StfsPackage::GetFileMagic(string pathInPackage)
{
    FileEntry entry = GetFileEntry(pathInPackage);

    // make sure the file is at least 4 bytes
    if (entry.fileSize < 4)
        return 0;

    // seek to the begining of the file in the package
    io->setPosition(BlockToAddress(entry.startingBlockNum));

    // read the magic
    return io->readDword();
}

void StfsPackage::ExtractFile(string pathInPackage, string outPath, void (*extractProgress)(void*, DWORD, DWORD), void *arg)
{
    // get the given path's file entry
    FileEntry entry = GetFileEntry(pathInPackage);

    // extract the file
    ExtractFile(&entry, outPath, extractProgress, arg);
}

void StfsPackage::ExtractFile(FileEntry *entry, string outPath, void (*extractProgress)(void*, DWORD, DWORD), void *arg)
{
    if (entry->nameLen == 0)
    {
        except.str(std::string());
        except << "STFS: File '" << entry->name << "' doesn't exist in the package.\n";
        throw except.str();
    }

    // create/truncate our out file
    FileIO outFile(outPath, true);

    // get the file size that we are extracting
    DWORD fileSize = entry->fileSize;

    // make a special case for files of size 0
    if (fileSize == 0)
    {
        outFile.close();

        // update progress if needed
        if (extractProgress != NULL)
            extractProgress(arg, 1, 1);

        return;
    }

    // check if all the blocks are consecutive
    if (entry->flags & 1)
    {
        // allocate 0xAA blocks of memory, for maximum efficiency, yo
        BYTE *buffer = new BYTE[0xAA000];

        // seek to the begining of the file
        DWORD startAddress = BlockToAddress(entry->startingBlockNum);
        io->setPosition(startAddress);

        // calculate the number of blocks to read before we hit a table
        DWORD blockCount = (ComputeLevel0BackingHashBlockNumber(entry->startingBlockNum) + blockStep[0]) - ((startAddress - firstHashTableAddress) >> 0xC);

        // pick up the change at the begining, until we hit a hash table
        if (entry->blocksForFile <= blockCount)
        {
            io->readBytes(buffer, entry->fileSize);
            outFile.write(buffer, entry->fileSize);

            // update progress if needed
            if (extractProgress != NULL)
                extractProgress(arg, entry->blocksForFile, entry->blocksForFile);

            outFile.close();

            // free the temp buffer
            delete[] buffer;
            return;
        }
        else
        {
            io->readBytes(buffer, blockCount << 0xC);
            outFile.write(buffer, blockCount << 0xC);

            // update progress if needed
            if (extractProgress != NULL)
                extractProgress(arg, blockCount, entry->blocksForFile);
        }

        // extract the blocks inbetween the tables
        DWORD tempSize = (entry->fileSize - (blockCount << 0xC));
        while (tempSize >= 0xAA000)
        {
            // skip past the hash table(s)
            DWORD currentPos = io->getPosition();
            io->setPosition(currentPos + GetHashTableSkipSize(currentPos));

            // read in the 0xAA blocks between the tables
            io->readBytes(buffer, 0xAA000);

            // write the bytes to the out file
            outFile.write(buffer, 0xAA000);

            tempSize -= 0xAA000;
            blockCount += 0xAA;

            // update progress if needed
            if (extractProgress != NULL)
                extractProgress(arg, blockCount, entry->blocksForFile);
        }

        // pick up the change at the end
        if (tempSize != 0)
        {
            // skip past the hash table(s)
            DWORD currentPos = io->getPosition();
            io->setPosition(currentPos + GetHashTableSkipSize(currentPos));

            // read in the extra crap
            io->readBytes(buffer, tempSize);

            // write it to the out file
            outFile.write(buffer, tempSize);

            // update progress if needed
            if (extractProgress != NULL)
                extractProgress(arg, entry->blocksForFile, entry->blocksForFile);
        }

        // free the temp buffer
        delete[] buffer;
    }
    else
    {
        // generate the block chain which we have to extract
        DWORD fullReadCounts = fileSize / 0x1000;

        fileSize -= (fullReadCounts * 0x1000);

        DWORD block = entry->startingBlockNum;

        // allocate data for the blocks
        BYTE data[0x1000];

        // read all the full blocks the file allocates
        for(DWORD i = 0; i < fullReadCounts; i++)
        {
            ExtractBlock(block, data);
            outFile.write(data, 0x1000);

            block = GetBlockHashEntry(block).nextBlock;

            // call the extract progress function if needed
            if (extractProgress != NULL)
                extractProgress(arg, i + 1, entry->blocksForFile);
        }

        // read the remaining data
        if (fileSize != 0)
        {
            ExtractBlock(block, data, fileSize);
            outFile.write(data, fileSize);

            // call the extract progress function if needed
            if (extractProgress != NULL)
                extractProgress(arg, entry->blocksForFile, entry->blocksForFile);
        }
    }

    // cleanup
    outFile.close();
}

DWORD StfsPackage::GetHashTableSkipSize(DWORD tableAddress)
{
    // convert the address to a true block number
    DWORD trueBlockNumber = (tableAddress - firstHashTableAddress) >> 0xC;

    // check if it's the first hash table
    if (trueBlockNumber == 0)
        return (0x1000 << packageSex);

    // check if it's the level 2 table, or above
    if (trueBlockNumber == blockStep[1])
        return (0x3000 << packageSex);
    else if (trueBlockNumber > blockStep[1])
        trueBlockNumber -= (blockStep[1] + (1 << packageSex));

    // check if it's at a level 1 table
    if (trueBlockNumber == blockStep[0] || trueBlockNumber % blockStep[1] == 0)
        return (0x2000 << packageSex);

    // otherwise, assume it's at a level 0 table
    return (0x1000 << packageSex);
}

FileEntry StfsPackage::GetFileEntry(string pathInPackage, bool checkFolders, FileEntry *newEntry)
{
    FileEntry entry;
    GetFileEntry(SplitString(pathInPackage, "\\"), &fileListing, &entry, newEntry, (newEntry != NULL), checkFolders);

    if (entry.nameLen == 0)
    {
        except.str(std::string());
        except << "STFS: File entry '" << pathInPackage << "' cannot be found in the package.\n";
        throw except.str();
    }

    return entry;
}

bool StfsPackage::FileExists(string pathInPackage)
{
    FileEntry entry;
    GetFileEntry(SplitString(pathInPackage, "\\"), &fileListing, &entry);
    return (entry.nameLen != 0);
}

void StfsPackage::GetFileEntry(vector<string> locationOfFile, FileListing *start, FileEntry *out, FileEntry *newEntry, bool updateEntry, bool checkFolders)
{
    bool found = false;

    // check to see if this is our last call
    if (locationOfFile.size() == 1)
    {
        // find the file
        for (DWORD i = 0; i < start->fileEntries.size(); i++)
            if (start->fileEntries.at(i).name == locationOfFile.at(0))
            {
                // update the entry
                if (updateEntry)
                {
                    start->fileEntries.at(i) = *newEntry;
                    io->setPosition(start->fileEntries.at(i).fileEntryAddress);
                    WriteFileEntry(&start->fileEntries.at(i));
                }

                // set the out value, and break
                if (out != NULL)
                    *out = start->fileEntries.at(i);

                found = true;
                break;
            }

        if (!found && checkFolders)
        {
            // find the file
            for (DWORD i = 0; i < start->folderEntries.size(); i++)
                if (start->folderEntries.at(i).folder.name == locationOfFile.at(0))
                {
                    // update the entry
                    if (updateEntry)
                    {
                        start->folderEntries.at(i).folder = *newEntry;
                        io->setPosition(start->folderEntries.at(i).folder.fileEntryAddress);
                        WriteFileEntry(&start->folderEntries.at(i).folder);
                    }

                    // set the out value, and break
                    if (out != NULL)
                        *out = start->folderEntries.at(i).folder;
                    found = true;
                    break;
                }
        }
    }
    else
    {
        // find the next folder
        for (DWORD i = 0; i < start->folderEntries.size(); i++)
            if (start->folderEntries.at(i).folder.name == locationOfFile.at(0))
            {
                // erase the found folder from the vector
                locationOfFile.erase(locationOfFile.begin());

                // recursively call GetFileEntry again with the updated vector, and break
                GetFileEntry(locationOfFile, &start->folderEntries.at(i), out, newEntry, updateEntry, checkFolders);
                found = true;
                break;
            }
    }

    // set the out to null if the file doesn't exist
    if (!found)
        out->nameLen = 0;
}

vector<string> StfsPackage::SplitString(string str, string delimeter)
{
    vector<string> splits;
    string temp;

    // find the next '\' in the string
    while (str.find(delimeter, 0) != string::npos)
    {
        // get the substring from the begginging of the string, to the next '\'
        size_t pos = str.find(delimeter, 0);
        temp = str.substr(0, pos);
        str.erase(0, pos + delimeter.size());

        // only add it if the substring is not null
        if (temp.size() > 0)
            splits.push_back(temp);
    }

    // add the last one
    splits.push_back(str);
    return splits;
}

void StfsPackage::AddToListing(FileListing *fullListing, FileListing *out)
{
    for (DWORD i = 0; i < fullListing->fileEntries.size(); i++)
    {
        // check if the file is a directory
        bool isDirectory = (fullListing->fileEntries.at(i).flags & 2);

        // make sure the file belongs to the current folder
        if (fullListing->fileEntries.at(i).pathIndicator == out->folder.entryIndex)
        {
            // add it if it's a file
            if (!isDirectory)
                out->fileEntries.push_back(fullListing->fileEntries.at(i));
            // if it's a directory and not the current directory, then add it
            else if (isDirectory && fullListing->fileEntries.at(i).entryIndex != out->folder.entryIndex)
            {
                FileListing fl;
                fl.folder = fullListing->fileEntries.at(i);
                out->folderEntries.push_back(fl);
            }
        }
    }

    // for every folder added, add the files to them
    for (DWORD i = 0; i < out->folderEntries.size(); i++)
        AddToListing(fullListing, &out->folderEntries.at(i));
}

HashTable StfsPackage::GetLevelNHashTable(DWORD index, Level lvl)
{
    HashTable toReturn;
    toReturn.level = lvl;

    // compute base address of the hash table
    toReturn.trueBlockNumber = ComputeLevelNBackingHashBlockNumber(index * dataBlocksPerHashTreeLevel[lvl], lvl);
    DWORD baseHashAddress = ((toReturn.trueBlockNumber << 0xC) + firstHashTableAddress);

    // adjust the hash address
    if (lvl < 0 || lvl > topLevel)
        throw string("STFS: Invalid level.\n");
    else if (lvl == topLevel)
        return topTable;
    else if (lvl + 1 == topLevel)
    {
        // add the hash offset to the base address so we use the correct table
        baseHashAddress += ((topTable.entries[index].status & 0x40) << 6);

        // calculate the number of entries in the requested table
        if (index + 1 == tablesPerLevel[lvl])
            toReturn.entryCount = (lvl == Zero) ? metaData->volumeDescriptor.allocatedBlockCount % 0xAA : tablesPerLevel[lvl - 1] % 0xAA;
        else
            toReturn.entryCount = 0xAA;
    }
    else
    {
        if (cached.trueBlockNumber != ComputeLevelNBackingHashBlockNumber(index * 0xAA, One))
            cached = GetLevelNHashTable(index % 0xAA, One);
        baseHashAddress += ((cached.entries[index % 0xAA].status & 0x40) << 6);

        // calculate the number of entries in the requested table
        if (index + 1 == tablesPerLevel[lvl])
            toReturn.entryCount = metaData->volumeDescriptor.allocatedBlockCount % 0xAA;
        else
            toReturn.entryCount = 0xAA;
    }

    // seek to the hash table requested
    toReturn.addressInFile = baseHashAddress;
    io->setPosition(toReturn.addressInFile);

    for (DWORD i = 0; i < toReturn.entryCount; i++)
    {
        io->readBytes(toReturn.entries[i].blockHash, 0x14);
        toReturn.entries[i].status = io->readByte();
        toReturn.entries[i].nextBlock = io->readInt24();
    }

    return toReturn;
}

DWORD StfsPackage::GetHashTableEntryCount(DWORD index, Level lvl)
{
    if (lvl < 0 || lvl > topLevel)
        throw string("STFS: Invalid level.\n");

    if (lvl == topLevel)
        return topTable.entryCount;
    else if (lvl + 1 == topLevel)
    {
        if (index + 1 == tablesPerLevel[lvl])
            return (lvl == Zero) ? metaData->volumeDescriptor.allocatedBlockCount % 0xAA : tablesPerLevel[lvl - 1] % 0xAA;
        else
            return 0xAA;
    }
    else
    {
        if (index + 1 == tablesPerLevel[lvl])
            return metaData->volumeDescriptor.allocatedBlockCount % 0xAA;
        else
            return 0xAA;
    }
}

void StfsPackage::Rehash()
{
    BYTE blockBuffer[0x1000];
    switch (topLevel)
    {
        case Zero:
            // set the position to the first data block in the file
            io->setPosition(BlockToAddress(0));
            // iterate through all of the data blocks
            for (DWORD i = 0; i < topTable.entryCount; i++)
            {
                // read in the current data block
                io->readBytes(blockBuffer, 0x1000);

                // hash the block
                HashBlock(blockBuffer, topTable.entries[i].blockHash);
            }

            break;

        case One:
            // loop through all of the level1 hash blocks
            for (DWORD i = 0; i < topTable.entryCount; i++)
            {
                // get the current level0 hash table
                HashTable level0Table = GetLevelNHashTable(i, Zero);

                // set the position to the first data block in this table
                io->setPosition(BlockToAddress(i * 0xAA));

                // iterate through all of the data blocks this table hashes
                for (DWORD x = 0; x < level0Table.entryCount; x++)
                {
                    // read in the current data block
                    io->readBytes(blockBuffer, 0x1000);

                    // hash the block
                    HashBlock(blockBuffer, level0Table.entries[x].blockHash);
                }

                // build the table for hashing and writing
                BuildTableInMemory(&level0Table, blockBuffer);

                // write the hash table back to the file
                io->setPosition(level0Table.addressInFile);
                io->write(blockBuffer, 0x1000);

                // hash the table
                HashBlock(blockBuffer, topTable.entries[i].blockHash);
            }
            break;

        case Two:
            // iterate through all of the level2 tables
            for (DWORD i = 0; i < topTable.entryCount; i++)
            {
                // get the current level1 hash table
                HashTable level1Table = GetLevelNHashTable(i, One);

                // iterate through all of the level0 tables hashed in this table
                for (DWORD x = 0; x < level1Table.entryCount; x++)
                {
                    // get the current level0 hash table
                    HashTable level0Table = GetLevelNHashTable((i * 0xAA) + x, Zero);

                    // set the position to the first data block in this table
                    io->setPosition(BlockToAddress((i * 0x70E4) + (x * 0xAA)));

                    // iterate through all of the data blocks hashed in this table
                    for (DWORD y = 0; y < level0Table.entryCount; y++)
                    {
                        // read the current data block
                        io->readBytes(blockBuffer, 0x1000);

                        // hash the data block
                        HashBlock(blockBuffer, level0Table.entries[y].blockHash);
                    }

                    // build the table for hashing and writing
                    BuildTableInMemory(&level0Table, blockBuffer);

                    // write the hash table back to the file
                    io->setPosition(level0Table.addressInFile);
                    io->write(blockBuffer, 0x1000);

                    // hash the table
                    HashBlock(blockBuffer, level1Table.entries[x].blockHash);
                }

                // build the table for hashing and writing
                BuildTableInMemory(&level1Table, blockBuffer);

                // write the number of blocks hashed by this table at the bottom of the table, MS why?
                DWORD blocksHashed;
                if (i + 1 == topTable.entryCount)
                    blocksHashed = (metaData->volumeDescriptor.allocatedBlockCount % 0x70E4 == 0) ? 0x70E4 : metaData->volumeDescriptor.allocatedBlockCount % 0x70E4;
                else
                    blocksHashed = 0x70E4;
                FileIO::swapEndian(&blocksHashed, 1, 4);
                ((DWORD*)&blockBuffer)[0x3FC] = blocksHashed;

                // write the hash table back to the file
                io->setPosition(level1Table.addressInFile);
                io->write(blockBuffer, 0x1000);

                // hash the table
                HashBlock(blockBuffer, topTable.entries[i].blockHash);
            }
            break;
    }

    // build table so we can write it to the file and hash it
    BuildTableInMemory(&topTable, blockBuffer);

    // write the number of blocks the table hashes at the bottom of the hash table, MS why?
    if (topTable.level >= One)
    {
        DWORD allocatedBlockCountSwapped = metaData->volumeDescriptor.allocatedBlockCount;
        FileIO::swapEndian(&allocatedBlockCountSwapped, 1, 4);
        ((DWORD*)&blockBuffer)[0x3FC] = allocatedBlockCountSwapped;
    }

    // hash the top table
    HashBlock(blockBuffer, metaData->volumeDescriptor.topHashTableHash);

    // write new hash block to the package
    io->setPosition(topTable.addressInFile);
    io->write(blockBuffer, 0x1000);

    // write new volume descriptor to the file
    metaData->WriteVolumeDescriptor();

    DWORD headerStart;

    // set the headerStart
    if (flags & StfsPackagePEC)
        headerStart = 0x23C;
    else
        headerStart = 0x344;

    // calculate header size / first hash table address
    DWORD calculated = ((metaData->headerSize + 0xFFF) & 0xF000);
    DWORD headerSize = calculated - headerStart;

    // read the data to hash
    BYTE *buffer = new BYTE[headerSize];
    io->setPosition(headerStart);
    io->readBytes(buffer, headerSize);

    // hash the header
    Botan::SHA_160 sha1;
    sha1.update(buffer, headerSize);
    sha1.final(metaData->headerHash);

    delete[] buffer;
    sha1.clear();

    metaData->WriteMetaData();
}

void StfsPackage::SwapTable(DWORD index, Level lvl)
{
    // only one table per set in female packages
    if (packageSex == StfsFemale)
        return;
    else if (index >= tablesPerLevel[lvl] || lvl > Two)
        throw string("STFS: Invaid parameters for swapping table.\n");

    // read in all the status's so that when we swap tables, the package isn't messed up
    DWORD entryCount = GetHashTableEntryCount(index, lvl);
    DWORD *tableStatuses = new DWORD[entryCount];

    // set the io to the beginning of the table
    DWORD tablePos = GetHashTableAddress(index, lvl) + 0x14;
    io->setPosition(tablePos);

    for (DWORD i = 0; i < entryCount; i++)
    {
        tableStatuses[i] = io->readDword();
        io->setPosition(tablePos + (i * 0x18));
    }

    // if the level requested to be swapped is the top level, we need to invert the '2' bit of the block seperation
    if (lvl == topTable.level)
    {
        metaData->volumeDescriptor.blockSeperation ^= 2;
        metaData->WriteVolumeDescriptor();
    }
    else
    {
        DWORD statusPosition = GetTableHashAddress(index, lvl) + 0x14;

        // read the status of the requested hash table
        io->setPosition(statusPosition);
        BYTE status = io->readByte();

        // invert the table used
        status ^= 0x40;

        // write it back to the table
        io->setPosition(statusPosition);
        io->write(status);
    }

    // retrieve the table address again since we swapped it
    tablePos = GetHashTableAddress(index, lvl) + 0x14;
    io->setPosition(tablePos);

    // write all the statuses to the other table
    for (DWORD i = 0; i < entryCount; i++)
    {
        io->write(tableStatuses[i]);
        io->setPosition(tablePos + (i * 0x18));
    }

    // good boys free their memory
    delete tableStatuses;

}

DWORD StfsPackage::GetHashTableAddress(DWORD index, Level lvl)
{
    // get the base address of the hash table requested
    DWORD baseAddress = GetBaseHashTableAddress(index, lvl);

    // only one table per index in female packages
    if (packageSex == StfsFemale)
        return baseAddress;

    // if the level requested is the top, then we need to reference the '2' bit of the block seperation
    else if (lvl == topTable.level)
        return baseAddress + ((metaData->volumeDescriptor.blockSeperation & 2) << 0xB);
    // otherwise, go to the table's hash to figure out which table to use
    else
    {
        io->setPosition(GetTableHashAddress(index, lvl) + 0x14);
        return baseAddress + ((io->readByte() & 0x40) << 6);
    }
}

DWORD StfsPackage::GetBaseHashTableAddress(DWORD index, Level lvl)
{
    return ((ComputeLevelNBackingHashBlockNumber(index * dataBlocksPerHashTreeLevel[lvl], lvl) << 0xC) + firstHashTableAddress);
}

DWORD StfsPackage::GetTableHashAddress(DWORD index, Level lvl)
{
    if(lvl >= topTable.level || lvl < Zero)
        throw string("STFS: Level is invalid. No parent hash address accessible.\n");

    // compute base address of the hash table
    DWORD baseHashAddress = GetBaseHashTableAddress(index / 0xAA, (Level)(lvl + 1));

    // add the hash offset to the base address so we use the correct table
    if (lvl + 1 == topLevel)
        baseHashAddress += ((metaData->volumeDescriptor.blockSeperation & 2) << 0xB);
    else
        baseHashAddress += ((topTable.entries[index].status & 0x40) << 6);

    return baseHashAddress + (index * 0x18);
}

void StfsPackage::Resign(string kvPath)
{
    FileIO kvIo(kvPath);
    kvIo.setPosition(0, ios_base::end);

    DWORD adder = 0;
    if (kvIo.getPosition() == 0x4000)
        adder = 0x10;

    DWORD headerStart, size, hashLoc, toSignLoc, consoleIDLoc;
    // set the headerStart
    if (flags & StfsPackagePEC)
    {
        headerStart = 0x23C;
        hashLoc = 0x228;
        size = 0xDC4;
        toSignLoc = 0x23C;
        consoleIDLoc = 0x275;
    }
    else
    {
        headerStart = 0x344;
        hashLoc = 0x32C;
        size = 0x118;
        toSignLoc = 0x22C;
        consoleIDLoc = 0x36C;
    }

    // calculate header size / first hash table address
    DWORD calculated = ((metaData->headerSize + 0xFFF) & 0xF000);
    DWORD headerSize = calculated - headerStart;

    // read the certificate
    kvIo.setPosition(0x9B8 + adder);
    metaData->certificate.publicKeyCertificateSize = kvIo.readWord();
    kvIo.readBytes(metaData->certificate.ownerConsoleID, 5);

    char tempPartNum[0x15];
    tempPartNum[0x14] = 0;
    kvIo.readBytes((BYTE*)tempPartNum, 0x14);
    metaData->certificate.ownerConsolePartNumber = string(tempPartNum);

    metaData->certificate.ownerConsoleType = (ConsoleType)kvIo.readByte();

    char tempGenDate[9] = {0};
    kvIo.readBytes((BYTE*)tempGenDate, 8);
    metaData->certificate.dateGeneration = string(tempGenDate);

    metaData->certificate.publicExponent = kvIo.readDword();
    kvIo.readBytes(metaData->certificate.publicModulus, 0x80);
    kvIo.readBytes(metaData->certificate.certificateSignature, 0x100);

    // read the keys for signing
    BYTE nData[0x80];
    BYTE pData[0x40];
    BYTE qData[0x40];

    kvIo.setPosition(0x298 + adder);
    kvIo.readBytes(nData, 0x80);
    kvIo.readBytes(pData, 0x40);
    kvIo.readBytes(qData, 0x40);

    // 8 byte swap all necessary keys
    XeCryptBnQw_SwapDwQwLeBe(nData, 0x80);
    XeCryptBnQw_SwapDwQwLeBe(pData, 0x40);
    XeCryptBnQw_SwapDwQwLeBe(qData, 0x40);

    // get the keys ready for signing
    Botan::BigInt n = Botan::BigInt::decode(nData, 0x80);
    Botan::BigInt p = Botan::BigInt::decode(pData, 0x40);
    Botan::BigInt q = Botan::BigInt::decode(qData, 0x40);

    Botan::AutoSeeded_RNG rng;
    Botan::RSA_PrivateKey pkey(rng, p, q, 0x10001, 0, n);

    // write the console id
    io->setPosition(consoleIDLoc);
    io->write(metaData->certificate.ownerConsoleID, 5);

    // read the data to hash
    BYTE *buffer = new BYTE[headerSize];
    io->setPosition(headerStart);
    io->readBytes(buffer, headerSize);

    // hash the header
    Botan::SHA_160 sha1;
    sha1.clear();
    sha1.update(buffer, headerSize);
    sha1.final(metaData->headerHash);

    delete[] buffer;

    io->setPosition(hashLoc);
    io->write(metaData->headerHash, 0x14);

    io->setPosition(toSignLoc);

    BYTE *dataToSign = new BYTE[size];
    io->readBytes(dataToSign, size);

#if defined __unix | defined __APPLE__
    Botan::PK_Signer signer(pkey, "EMSA3(SHA-160)");
#elif _WIN32
    Botan::PK_Signer signer(pkey, Botan::get_emsa("EMSA3(SHA-160)"));
#endif

    Botan::SecureVector<Botan::byte> signature = signer.sign_message((unsigned char*)dataToSign, size, rng);

    // 8 byte swap the new signature
    XeCryptBnQw_SwapDwQwLeBe(signature, 0x80);

    // reverse the new signature every 8 bytes
    for (int i = 0; i < 0x10; i++)
        FileIO::swapEndian(&signature[i * 8], 1, 8);

    // write the certficate
    memcpy(metaData->certificate.signature, signature, 0x80);
    metaData->WriteCertificate();

    delete[] dataToSign;
}


void StfsPackage::SetBlockStatus(DWORD blockNum, BlockStatusLevelZero status)
{
    if (blockNum >= metaData->volumeDescriptor.allocatedBlockCount)
         throw string("STFS: Reference to illegal block number.\n");

    DWORD statusAddress = GetHashAddressOfBlock(blockNum) + 0x14;
    io->setPosition(statusAddress);
    io->write((BYTE)status);
}

void StfsPackage::HashBlock(BYTE *block, BYTE *outBuffer)
{
    // hash the block
    Botan::SHA_160 sha1;
    sha1.clear();
    sha1.update(block, 0x1000);
    sha1.final(outBuffer);
}

void StfsPackage::BuildTableInMemory(HashTable *table, BYTE *outBuffer)
{
    memset(outBuffer, 0, 0x1000);
    for (DWORD i = 0; i < table->entryCount; i++)
    {
        // copy the hash over
        memcpy(outBuffer + (i * 0x18), table->entries[i].blockHash, 0x15);

        // copy over the next block
        INT24 swappedValue = table->entries[i].nextBlock;
        FileIO::swapEndian(&swappedValue, 1, 4);
        swappedValue >>= 8;
        memcpy((outBuffer + i * 0x18) + 0x15, &swappedValue, 3);
    }
}

void StfsPackage::XeCryptBnQw_SwapDwQwLeBe(BYTE *data, DWORD length)
{
    if (length % 8 != 0)
        throw string("STFS: length is not divisible by 8.\n");

    for (DWORD i = 0; i < length / 2; i += 8)
    {
        BYTE temp[8];
        memcpy(temp, &data[i], 8);

        BYTE temp2[8];
        memcpy(temp2, &data[length - i - 8], 8);

        memcpy(&data[i], temp2, 8);
        memcpy(&data[length - i - 8], temp, 8);
    }
}

void StfsPackage::RemoveFile(FileEntry entry)
{
    bool found = false;

    vector<FileEntry> files, folders;
    GenerateRawFileListing(&fileListing, &files, &folders);

    // remove the file from the listing
    for (DWORD i = 0; i < files.size(); i++)
        if (files.at(i).name == entry.name && files.at(i).pathIndicator == entry.pathIndicator)
        {
            files.erase(files.begin() + i);
            found = true;
            break;
        }

    // make sure the file was found in the package
    if (!found)
        throw string("STFS: File could not be deleted because it doesn't exist in the package.\n");

    // set the status of every allocated block to unallocated
    DWORD blockToDeallocate = entry.startingBlockNum;
    while (blockToDeallocate != INT24_MAX)
    {
        SetBlockStatus(blockToDeallocate, Unallocated);
        blockToDeallocate = GetBlockHashEntry(blockToDeallocate).nextBlock;
    }

    // update the file listing
    WriteFileListing(true, &files, &folders);
}

void StfsPackage::WriteFileListing(bool usePassed, vector<FileEntry> *outFis, vector<FileEntry> *outFos)
{
    // get the raw file listing
    vector<FileEntry> outFiles, outFolders;

    if (!usePassed)
        GenerateRawFileListing(&fileListing, &outFiles, &outFolders);
    else
    {
        outFiles = *outFis;
        outFolders = *outFos;
    }

    // take out the 'Root' directory
    outFolders.erase(outFolders.begin());

    // initialize the folders map (used in new path indicators)
    std::map<INT24, int> folders;
    folders[0xFFFF] = 0xFFFF;

    bool alwaysAllocate = false, firstCheck = true;

    // go to the block where the file listing begins (for overwriting)
    DWORD block = metaData->volumeDescriptor.fileTableBlockNum;
    io->setPosition(BlockToAddress(block));

    DWORD outFileSize = outFolders.size();

    // add all folders to the folders map
    for (DWORD i = 0; i < outFileSize; i++)
        folders[outFolders.at(i).entryIndex] = i;

    // write the folders to the listing
    for (DWORD i = 0; i < outFolders.size(); i++)
    {
        // check to see if we need to go to the next block
        if (firstCheck)
            firstCheck = false;
        else if ((i + 1) % 0x40 == 0)
        {
            // check if we need to allocate a new block
            INT24 nextBlock;
            if (alwaysAllocate)
            {
                nextBlock = AllocateBlock();

                // if so, set the current block pointing to the next one
                SetNextBlock(block, nextBlock);
            }
            else
            {
                // see if a block was already allocated with the previous table
                nextBlock = GetBlockHashEntry(block).nextBlock;

                // if not, allocate one and make it so it always allocates
                if (nextBlock == INT24_MAX)
                {
                    nextBlock = AllocateBlock();
                    SetNextBlock(block, nextBlock);
                    alwaysAllocate = true;
                }
            }

            // go to the next block position
            block = nextBlock;
            io->setPosition(BlockToAddress(block));
        }

        // set the correct path indicator
        outFolders.at(i).pathIndicator = folders[outFolders.at(i).pathIndicator];

        // write the file (folder) entry to file
        WriteFileEntry(&outFolders.at(i));
    }

    // same as above
    int outFoldersAndFilesSize = outFileSize + outFiles.size();
    for(int i = outFileSize; i < outFoldersAndFilesSize; i++)
    {
        if (firstCheck)
            firstCheck = false;
        else if (i % 0x40 == 0)
        {
            INT24 nextBlock;
            if (alwaysAllocate)
            {
                nextBlock = AllocateBlock();
                SetNextBlock(block, nextBlock);
            }
            else
            {
                nextBlock = GetBlockHashEntry(block).nextBlock;
                if (nextBlock == INT24_MAX)
                {
                    nextBlock = AllocateBlock();
                    SetNextBlock(block, nextBlock);
                    alwaysAllocate = true;
                }
            }

            block = nextBlock;
            io->setPosition(BlockToAddress(block));
        }

        outFiles.at(i - outFileSize).pathIndicator = folders[outFiles.at(i - outFileSize).pathIndicator];
        WriteFileEntry(&outFiles.at(i - outFileSize));
    }

    // write remaining null bytes
    DWORD remainingEntries = (outFoldersAndFilesSize % 0x40);
    int remainer = 0;
    if (remainingEntries > 0)
        remainer = (0x40 - remainingEntries) * 0x40;
    BYTE *nullBytes = new BYTE[remainer];
    memset(nullBytes, 0, remainer);
    io->write(nullBytes, remainer);

    // update the file table block count and write it to file
    metaData->volumeDescriptor.fileTableBlockCount = (outFoldersAndFilesSize / 0x40) + 1;
    if (outFoldersAndFilesSize % 0x40 == 0 && outFoldersAndFilesSize != 0)
        metaData->volumeDescriptor.fileTableBlockCount--;
    metaData->WriteVolumeDescriptor();

    ReadFileListing();
}

void StfsPackage::SetNextBlock(DWORD blockNum, INT24 nextBlockNum)
{
    if (blockNum >= metaData->volumeDescriptor.allocatedBlockCount)
        throw string("STFS: Reference to illegal block number.\n");

    DWORD hashLoc = GetHashAddressOfBlock(blockNum) + 0x15;
    io->setPosition(hashLoc);
    io->write((INT24)nextBlockNum);

    if (topLevel == Zero)
        topTable.entries[blockNum].nextBlock = nextBlockNum;

    io->flush();
}

void StfsPackage::WriteFileEntry(FileEntry *entry)
{
    // update the name length so it matches the string
    entry->nameLen = entry->name.length();

    if (entry->nameLen > 0x28)
        throw string("STFS: File entry name length cannot be greater than 40(0x28) characters.\n");

    // put the flags and name length into one byte
    BYTE nameLengthAndFlags = entry->nameLen | (entry->flags << 6);

    // write the entry
    io->write(entry->name, 0x28);
    io->write(nameLengthAndFlags);
    io->write(entry->blocksForFile, LittleEndian);
    io->write(entry->blocksForFile, LittleEndian);
    io->write(entry->startingBlockNum, LittleEndian);
    io->write(entry->pathIndicator);
    io->write(entry->fileSize);
    io->write(entry->createdTimeStamp);
    io->write(entry->accessTimeStamp);
}

void StfsPackage::RemoveFile(string pathInPackage)
{
    RemoveFile(GetFileEntry(pathInPackage));
}

INT24 StfsPackage::AllocateBlock()
{
    // reset the cached table
    cached.addressInFile = 0;
    cached.entryCount = 0;
    cached.level = (Level)-1;
    cached.trueBlockNumber = 0xFFFFFFFF;

    DWORD lengthToWrite = 0xFFF;

    // update the allocated block count
    metaData->volumeDescriptor.allocatedBlockCount++;

    // recalculate the hash table counts to see if we need to make any new tables
    DWORD recalcTablesPerLevel[3];
    recalcTablesPerLevel[0] = (metaData->volumeDescriptor.allocatedBlockCount / 0xAA) + ((metaData->volumeDescriptor.allocatedBlockCount % 0xAA != 0) ? 1 : 0);
    recalcTablesPerLevel[1] = (recalcTablesPerLevel[0] / 0xAA) + ((recalcTablesPerLevel[0] % 0xAA != 0 && metaData->volumeDescriptor.allocatedBlockCount > 0xAA) ? 1 : 0);
    recalcTablesPerLevel[2] = (recalcTablesPerLevel[1] / 0xAA) + ((recalcTablesPerLevel[1] % 0xAA != 0 && metaData->volumeDescriptor.allocatedBlockCount > 0x70E4) ? 1 : 0);

    // allocate memory for hash tables if needed
    for (int i = 2; i >= 0; i--)
    {
        if (recalcTablesPerLevel[i] != tablesPerLevel[i])
        {
            lengthToWrite += (packageSex + 1) * 0x1000;
            tablesPerLevel[i] = recalcTablesPerLevel[i];

            // update top level hash table if needed
            if ((i + 1) == topLevel)
            {
                topTable.entryCount++;
                topTable.entries[topTable.entryCount - 1].status = 0;
                topTable.entries[topTable.entryCount - 1].nextBlock = INT24_MAX;

                // write it to the file
                io->setPosition(topTable.addressInFile + ((tablesPerLevel[i] - 1) * 0x18) + 0x15);
                io->write((INT24)INT24_MAX);
            }
        }
    }

    // allocate the necessary memory
    io->setPosition(lengthToWrite, ios_base::end);
    io->write((BYTE)0);

    // if the top level changed, then we need to re-load the top table
    Level newTop = CalcualateTopLevel();
    if (topLevel != newTop)
    {
        topLevel = newTop;
        topTable.level = topLevel;

        DWORD blockOffset = metaData->volumeDescriptor.blockSeperation & 2;
        metaData->volumeDescriptor.blockSeperation &= 0xFD;
        topTable.addressInFile = GetHashTableAddress(0, topLevel);
        topTable.entryCount = 2;
        topTable.trueBlockNumber = ComputeLevelNBackingHashBlockNumber(0, topLevel);

        // clear the top table
        memset(topTable.entries, 0, sizeof(HashEntry) * 0xAA);

        topTable.entries[0].status = blockOffset << 5;
        io->setPosition(topTable.addressInFile + 0x14);
        io->write((BYTE)topTable.entries[0].status);

        // clear the top hash offset
        metaData->volumeDescriptor.blockSeperation &= 0xFD;

    }

    // write the block status
    io->setPosition(GetHashAddressOfBlock(metaData->volumeDescriptor.allocatedBlockCount - 1) + 0x14);
    io->write((BYTE)Allocated);

    if (topLevel == Zero)
    {
        topTable.entryCount++;
        topTable.entries[metaData->volumeDescriptor.allocatedBlockCount - 1].status = (BYTE)Allocated;
        topTable.entries[metaData->volumeDescriptor.allocatedBlockCount - 1].nextBlock = INT24_MAX;
    }

    // terminate the chain
    io->write((INT24)0xFFFFFF);

    metaData->WriteVolumeDescriptor();
    return metaData->volumeDescriptor.allocatedBlockCount - 1;
}

DWORD StfsPackage::GetBlocksUntilNextHashTable(DWORD currentBlock)
{
    return (ComputeLevel0BackingHashBlockNumber(currentBlock) + blockStep[0]) - ((BlockToAddress(currentBlock) - firstHashTableAddress) >> 0xC);
}

INT24 StfsPackage::AllocateBlocks(DWORD blockCount)
{
    INT24 returnValue = metaData->volumeDescriptor.allocatedBlockCount;

    // figure out how far away the next hash table set is
    DWORD blocksUntilTable = GetBlocksUntilNextHashTable(metaData->volumeDescriptor.allocatedBlockCount);

    // create a hash block of all statuses set to allocated, for fast writing
    BYTE allocatedHashBlock[0x1000];
    for (DWORD i = 0; i < 0xAA; i++)
        allocatedHashBlock[0x14 + (i * 0x18)] = 0x80;

    // allocate the amount before the hash table

    // allocate the memory in the file
    io->setPosition((((blockCount <= blocksUntilTable) ? blockCount : blocksUntilTable) << 0xC) - 1, ios_base::end);
    io->write((BYTE)0);

    // set blocks to allocated in hash table
    io->setPosition(BlockToAddress((metaData->volumeDescriptor.allocatedBlockCount - (0xAA - blocksUntilTable))) - (0x1000 << packageSex) + (metaData->volumeDescriptor.allocatedBlockCount * 0x18));
    io->write(allocatedHashBlock, blocksUntilTable * 0x18);

    // update the allocated block count
    metaData->volumeDescriptor.allocatedBlockCount += ((blockCount <= blocksUntilTable) ? blockCount : blocksUntilTable);

    // allocate memory the hash table
    io->setPosition(GetHashTableSkipSize(metaData->volumeDescriptor.allocatedBlockCount) - 1, ios_base::end);
    io->write((BYTE)0);

    blockCount -= blocksUntilTable;

    // allocate all of the full sets
    while (blockCount >= 0xAA)
    {
        // allocate the memory in the file
        io->setPosition(0xAA000 + GetHashTableSkipSize(metaData->volumeDescriptor.allocatedBlockCount + 0xAA) - 1, ios_base::end);
        io->write((BYTE)0);

        // set all the blocks to allocated
        io->setPosition(BlockToAddress(metaData->volumeDescriptor.allocatedBlockCount) - (0x1000 << packageSex));
        io->write(allocatedHashBlock, 0x1000);

        // update the values
        metaData->volumeDescriptor.allocatedBlockCount += 0xAA;
        blockCount -= 0xAA;
    }

    if (blockCount > 0)
    {
        // allocate the extra
        io->setPosition(GetHashTableSkipSize(metaData->volumeDescriptor.allocatedBlockCount + 0xAA) + (blockCount << 0xC) - 1, ios_base::end);
        io->write((BYTE)0);

        // set all the blocks to allocated
        io->setPosition(BlockToAddress(metaData->volumeDescriptor.allocatedBlockCount) - (0x1000 << packageSex));
        io->write(allocatedHashBlock, blockCount * 0x18);
    }

    metaData->WriteVolumeDescriptor();

    // if the top level changed, then we need to re-load the top table
    Level newTop = CalcualateTopLevel();
    if (topLevel != newTop)
    {
        topLevel = newTop;
        topTable.level = topLevel;

        DWORD blockOffset = metaData->volumeDescriptor.blockSeperation & 2;
        metaData->volumeDescriptor.blockSeperation &= 0xFD;
        topTable.addressInFile = GetHashTableAddress(0, topLevel);
        topTable.entryCount = 2;

        // clear the top table
        memset(topTable.entries, 0, sizeof(HashEntry) * 0xAA);

        topTable.entries[0].status = blockOffset << 5;
        io->setPosition(topTable.addressInFile + 0x14);
        io->write((BYTE)topTable.entries[0].status);

        // clear the top hash offset
        metaData->volumeDescriptor.blockSeperation &= 0xFD;

    }

    return returnValue;
}

void StfsPackage::FindDirectoryListing(vector<string> locationOfDirectory, FileListing *start, FileListing **out)
{
    if(locationOfDirectory.size() == 0)
        *out = start;

    bool finalLoop = (locationOfDirectory.size() == 1);
    for (DWORD i = 0; i < start->folderEntries.size(); i++)
    {
        if (start->folderEntries.at(i).folder.name == locationOfDirectory.at(0))
        {
            locationOfDirectory.erase(locationOfDirectory.begin());
            if (finalLoop)
                *out = &start->folderEntries.at(i);
            else
                for (DWORD i = 0; i < start->folderEntries.size(); i++)
                    if(*out == NULL)
                        FindDirectoryListing(locationOfDirectory, &start->folderEntries.at(i), out);
                    else
                        break;

            break;
        }
    }
}

void StfsPackage::UpdateEntry(string pathInPackage, FileEntry entry)
{
    GetFileEntry(SplitString(pathInPackage, "\\"), &fileListing, NULL, &entry, true);
}

FileEntry StfsPackage::InjectFile(string path, string pathInPackage, void(*injectProgress)(void*, DWORD, DWORD), void *arg)
{
    if(FileExists(pathInPackage))
        throw string("STFS: File already exists in the package.\n");

    // split the string and open a io
    vector<string> split = SplitString(pathInPackage, "\\");
    FileListing *folder = NULL;

    int size = split.size();
    string fileName;
    if(size > 1)
    {
        // get the name
        fileName = split.at(size - 1);
        split.erase(split.begin() + (size - 1));

        // find the directory we'd like to inject to
        FindDirectoryListing(split, &fileListing, &folder);
        if(folder == NULL)
            throw string("STFS: The given folder could not be found.\n");
    }
    else
    {
        fileName = pathInPackage;
        folder = &fileListing;
    }

    FileIO fileIn(path);

    fileIn.setPosition(0, ios_base::end);
    DWORD fileSize = fileIn.getPosition();
    fileIn.setPosition(0);

    // set up the entry
    FileEntry entry;
    entry.name = fileName;

    if (fileName.length() > 0x28)
        throw string("STFS: File entry name length cannot be greater than 40(0x28) characters.\n");

    // update the progress if needed
    if (injectProgress != NULL)
        injectProgress(arg, 0, entry.blocksForFile);

    entry.fileSize = fileSize;
    entry.flags = ConsecutiveBlocks;
    entry.pathIndicator = folder->folder.entryIndex;
    entry.startingBlockNum = INT24_MAX;
    entry.blocksForFile = ((fileSize + 0xFFF) & 0xFFFFFFF000) >> 0xC;
    entry.createdTimeStamp = MSTimeToDWORD(TimetToMSTime(time(NULL)));
    entry.accessTimeStamp = entry.createdTimeStamp;

    INT24 block = 0;
    INT24 prevBlock = INT24_MAX;
    DWORD counter = 0;
    BYTE data[0x1000];
    while(fileSize >= 0x1000)
    {
        block = AllocateBlock();


        if (entry.startingBlockNum == INT24_MAX)
            entry.startingBlockNum = block;

        if (prevBlock != INT24_MAX)
            SetNextBlock(prevBlock, block);

        prevBlock = block;

        // read the data;
        fileIn.readBytes(data, 0x1000);

        io->setPosition(BlockToAddress(block));
        io->write(data, 0x1000);

        fileSize -= 0x1000;

        // update the progress if needed
        if (injectProgress != NULL)
            injectProgress(arg, ++counter, entry.blocksForFile);
    }

    if(fileSize != 0)
    {
        block = AllocateBlock();

        if (entry.startingBlockNum == INT24_MAX)
            entry.startingBlockNum = block;       

        if (prevBlock != INT24_MAX)
            SetNextBlock(prevBlock, block);

        BYTE *data = new BYTE[fileSize];
        fileIn.readBytes(data, fileSize);
        io->setPosition(BlockToAddress(block));
        io->write(data, fileSize);

        fileSize = 0;

        // update the progress if needed
        if (injectProgress != NULL)
            injectProgress(arg, entry.blocksForFile, entry.blocksForFile);

        // free the heap memory
        delete[] data;
    }
    fileIn.close();

    SetNextBlock(block, INT24_MAX);

    folder->fileEntries.push_back(entry);
    WriteFileListing();

    if (topLevel == Zero)
    {
        io->setPosition(topTable.addressInFile);

        topTable.entryCount = metaData->volumeDescriptor.allocatedBlockCount;

        for (DWORD i = 0; i < topTable.entryCount; i++)
        {
            io->readBytes(topTable.entries[i].blockHash, 0x14);
            topTable.entries[i].status = io->readByte();
            topTable.entries[i].nextBlock = io->readInt24();
        }
    }
    return entry;
}

FileEntry StfsPackage::InjectData(BYTE *data, DWORD length, string pathInPackage, void (*injectProgress)(void *, DWORD, DWORD), void *arg)
{
    if(FileExists(pathInPackage))
        throw string("STFS: File already exists in the package.\n");

    // split the string and open a io
    vector<string> split = SplitString(pathInPackage, "\\");
    FileListing *folder = NULL;

    int size = split.size();
    string fileName;
    if(size > 1)
    {
        // get the name
        fileName = split.at(size - 1);
        split.erase(split.begin() + (size - 1));

        // find the directory we'd like to inject to
        FindDirectoryListing(split, &fileListing, &folder);
        if(folder == NULL)
            throw string("STFS: The given folder could not be found.\n");
    }
    else
    {
        fileName = pathInPackage;
        folder = &fileListing;
    }

    DWORD fileSize = length;

    // set up the entry
    FileEntry entry;
    entry.name = fileName;
    entry.fileSize = fileSize;
    entry.flags = 0;
    entry.pathIndicator = folder->folder.entryIndex;
    entry.startingBlockNum = INT24_MAX;
    entry.blocksForFile = ((fileSize + 0xFFF) & 0xFFFFFFF000) >> 0xC;

    INT24 block = 0;
    INT24 prevBlock = INT24_MAX;
    DWORD counter = 0;
    while(fileSize >= 0x1000)
    {
        block = AllocateBlock();

        if (entry.startingBlockNum == INT24_MAX)
            entry.startingBlockNum = block;

        if (prevBlock != INT24_MAX)
            SetNextBlock(prevBlock, block);

        prevBlock = block;

        // read the data
        BYTE *dataBlock = data + (counter++ * 0x1000);

        io->setPosition(BlockToAddress(block));
        io->write(dataBlock, 0x1000);

        fileSize -= 0x1000;

        // update the progress if needed
        if (injectProgress != NULL)
            injectProgress(arg, counter, entry.blocksForFile);
    }

    if(fileSize != 0)
    {
        block = AllocateBlock();

        if (entry.startingBlockNum == INT24_MAX)
            entry.startingBlockNum = block;

        if (prevBlock != INT24_MAX)
            SetNextBlock(prevBlock, block);

        BYTE *blockData = data + (length - fileSize);
        io->setPosition(BlockToAddress(block));
        io->write(blockData, fileSize);

        fileSize = 0;

        // update the progress if needed
        if (injectProgress != NULL)
            injectProgress(arg, entry.blocksForFile, entry.blocksForFile);
    }

    SetNextBlock(block, INT24_MAX);

    folder->fileEntries.push_back(entry);
    WriteFileListing();

    if (topLevel == Zero)
    {
        io->setPosition(topTable.addressInFile);

        topTable.entryCount = metaData->volumeDescriptor.allocatedBlockCount;

        for (DWORD i = 0; i < topTable.entryCount; i++)
        {
            io->readBytes(topTable.entries[i].blockHash, 0x14);
            topTable.entries[i].status = io->readByte();
            topTable.entries[i].nextBlock = io->readInt24();
        }
    }

    return entry;
}

void StfsPackage::ReplaceFile(string path, FileEntry *entry, string pathInPackage, void (*replaceProgress)(void *, DWORD, DWORD), void *arg)
{
    if (entry->nameLen == 0)
       throw string("STFS: File doesn't exists in the package.\n");

    FileIO fileIn(path);

    fileIn.setPosition(0, ios_base::end);
    DWORD fileSize = fileIn.getPosition();
    fileIn.setPosition(0);

    // set up the entry
    entry->fileSize = fileSize;
    entry->blocksForFile = ((fileSize + 0xFFF) & 0xFFFFFFF000) >> 0xC;

    DWORD block = entry->startingBlockNum;
    io->setPosition(BlockToAddress(block));

    DWORD fullReads = fileSize / 0x1000;
    bool first = true, alwaysAllocate = false;

    // write the folders to the listing
    for (DWORD i = 0; i < fullReads; i++)
    {
        if (!first)
        {
            // check if we need to allocate a new block
            INT24 nextBlock;
            if (alwaysAllocate)
            {
                nextBlock = AllocateBlock();

                // if so, set the current block pointing to the next one
                SetNextBlock(block, nextBlock);
            }
            else
            {
                // see if a block was already allocated with the previous table
                nextBlock = GetBlockHashEntry(block).nextBlock;

                // if not, allocate one and make it so it always allocates
                if (nextBlock == INT24_MAX)
                {
                    nextBlock = AllocateBlock();
                    SetNextBlock(block, nextBlock);
                    alwaysAllocate = true;
                }
            }

            // go to the next block position
            block = nextBlock;
            io->setPosition(BlockToAddress(block));
        }
        else
            first = false;

        // read in the data
        BYTE toWrite[0x1000];
        fileIn.readBytes(toWrite, 0x1000);

        // write the data
        io->write(toWrite, 0x1000);

        // update the progress if needed
        if (replaceProgress != NULL)
            replaceProgress(arg, i, entry->blocksForFile);
    }

    DWORD remainder = fileSize % 0x1000;
    if (remainder != 0)
    {
        INT24 nextBlock;
        if(!first)
        {
            // check if we need to allocate a new block
            if (alwaysAllocate)
            {
                nextBlock = AllocateBlock();

                // if so, set the current block pointing to the next one
                SetNextBlock(block, nextBlock);
            }
            else
            {
                // see if a block was already allocated with the previous table
                nextBlock = GetBlockHashEntry(block).nextBlock;

                // if not, allocate one and make it so it always allocates
                if (nextBlock == INT24_MAX)
                {
                    nextBlock = AllocateBlock();
                    SetNextBlock(block, nextBlock);
                    alwaysAllocate = true;
                }
            }

            block = nextBlock;
        }
        // go to the next block position
        io->setPosition(BlockToAddress(block));

        BYTE *toWrite = new BYTE[remainder];
        fileIn.readBytes(toWrite, remainder);

        io->write(toWrite, remainder);

        delete[] toWrite;
    }

    // update the progress if needed
    if (replaceProgress != NULL)
        replaceProgress(arg, entry->blocksForFile, entry->blocksForFile);

    SetNextBlock(block, INT24_MAX);

    entry->flags &= 0x2;

    io->setPosition(entry->fileEntryAddress + 0x28);
    io->write((BYTE)(entry->nameLen | (entry->flags << 6)));
    io->write(entry->blocksForFile, LittleEndian);
    io->write(entry->blocksForFile, LittleEndian);

    io->setPosition(entry->fileEntryAddress + 0x34);
    io->write(entry->fileSize);
    UpdateEntry(pathInPackage, *entry);

    if (topLevel == Zero)
    {
        io->setPosition(topTable.addressInFile);

        for (DWORD i = 0; i < topTable.entryCount; i++)
        {
            io->readBytes(topTable.entries[i].blockHash, 0x14);
            topTable.entries[i].status = io->readByte();
            topTable.entries[i].nextBlock = io->readInt24();
        }
    }
}

void StfsPackage::ReplaceFile(string path, string pathInPackage, void (*replaceProgress)(void *, DWORD, DWORD), void *arg)
{
    FileEntry entry = GetFileEntry(pathInPackage);
    ReplaceFile(path, &entry, pathInPackage, replaceProgress, arg);
}

void StfsPackage::RenameFile(string newName, string pathInPackage)
{
    FileEntry entry = GetFileEntry(pathInPackage, true);
    entry.name = newName;

    // update the entry in memory
    GetFileEntry(pathInPackage, true, &entry);

    io->setPosition(entry.fileEntryAddress);
    WriteFileEntry(&entry);
}

void StfsPackage::Close()
{
    io->close();
}

void StfsPackage::CreateFolder(string pathInPackage)
{
    // split the string and open a io
    vector<string> split = SplitString(pathInPackage, "\\");

    FileListing *folder = NULL;
    FindDirectoryListing(split, &fileListing, &folder);
    if (folder != NULL)
        throw string("STFS: Directory already exists in the package.\n");

    int size = split.size();
    string fileName;
    if(size > 1)
    {
        // get the name
        fileName = split.at(size - 1);
        split.erase(split.begin() + (size - 1));

        // find the directory we'd like to inject to
        FindDirectoryListing(split, &fileListing, &folder);
        if(folder == NULL)
            throw string("STFS: The given folder could not be found.\n");
    }
    else
    {
        fileName = pathInPackage;
        folder = &fileListing;
    }

    // set up the entry
    FileEntry entry;
    entry.name = fileName;

    if (fileName.length() > 0x28)
        throw string("STFS: File entry name length cannot be greater than 40(0x28) characters.\n");

    entry.nameLen = fileName.length();
    entry.fileSize = 0;
    entry.flags = Folder;
    entry.pathIndicator = folder->folder.entryIndex;
    entry.startingBlockNum = 0;
    entry.blocksForFile = 0;
    entry.createdTimeStamp = MSTimeToDWORD(TimetToMSTime(time(NULL)));
    entry.accessTimeStamp = entry.createdTimeStamp;

    FileListing newFolder;
    newFolder.folder = entry;

    // add the entry to the listing
    folder->folderEntries.push_back(newFolder);
    WriteFileListing();
}

void StfsPackage::GenerateRawFileListing(FileListing *in, vector<FileEntry> *outFiles, vector<FileEntry> *outFolders)
{
    int fiEntries = in->fileEntries.size();
    int foEntries = in->folderEntries.size();

    for (int i = 0; i < fiEntries; i++)
        outFiles->push_back(in->fileEntries.at(i));

    outFolders->push_back(in->folder);

    for (int i = 0; i < foEntries; i++)
        GenerateRawFileListing(&in->folderEntries.at(i), outFiles, outFolders);
}

StfsPackage::~StfsPackage(void)
{
    io->close();
    delete io;
    delete metaData;
}
