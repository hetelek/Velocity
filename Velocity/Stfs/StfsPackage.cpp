#include "StfsPackage.h"
#include "StfsMetaData.h"

#ifdef STFS_DEBUG
#include <stdio.h>
#include <QDebug>
#endif

StfsPackage::StfsPackage(string packagePath, bool isPEC) : isPEC(isPEC)
{
    io = new FileIO(packagePath);
    metaData = new StfsMetaData(io, isPEC);

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
    topTable.entries = new HashEntry[topTable.entryCount];

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
    return isPEC;
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

DWORD StfsPackage::ComputeLevel2BackingHashBlockNumber(DWORD blockNum)
{
    return blockStep[1];
}

DWORD StfsPackage::GetHashAddressOfBlock(DWORD blockNum)
{
    DWORD hashAddr = (ComputeLevel0BackingHashBlockNumber(blockNum) << 0xC) + firstHashTableAddress;
    hashAddr += (blockNum % 0xAA) * 0x18;

    switch (topLevel)
    {
        case 0:
            return hashAddr + ((metaData->volumeDescriptor.blockSeperation & 2) << 0xB);
        case 1:
            return hashAddr + ((topTable.entries[blockNum / 0xAA].status & 0x40) << 6);
        case 2:
            DWORD level1Off = ((topTable.entries[blockNum / 0x70E4].status & 0x40) << 6);
            DWORD pos = ((ComputeLevel1BackingHashBlockNumber(blockNum) << 0xC) + firstHashTableAddress + level1Off) +( (blockNum % 0xAA) * 0x18);
            io->setPosition(pos + 0x14);
            return hashAddr + ((io->readByte() & 0x40) << 6);
    }
}

HashEntry StfsPackage::GetBlockHashEntry(DWORD blockNum)
{
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

            // set the current positionz
            fe.fileEntryAddress = currentAddr + (i * 0x40);

            // calculate the entry index (in the file listing)
            fe.entryIndex = (x * 0x40) + i;

            // read the name, if the length is 0 then break
            fe.name = io->readString(0x28);
            if (fe.name.length() == 0)
                break;

            // read the name length
            fe.nameLen = io->readByte();

            // check for a mismatch in the total allocated blocks for the file
            fe.blocksForFile = io->readInt24(LittleEndian);
            io->setPosition(3, ios_base::cur);

            // read more information
            fe.startingBlockNum = io->readInt24(LittleEndian);
            fe.pathIndicator = io->readWord();
            fe.fileSize = io->readDword();
            fe.updateTimeStamp = io->readDword();
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

void StfsPackage::ExtractFile(string pathInPackage, string outPath)
{
    // get the given path's file entry
    FileEntry entry = GetFileEntry(pathInPackage);

    // extract the file
    ExtractFile(&entry, outPath);
}

void StfsPackage::ExtractFile(FileEntry *entry, string outPath)
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
    }

    // read the remaining data
    if (fileSize != 0)
    {
        ExtractBlock(block, data, fileSize);
        outFile.write(data, fileSize);
    }

    // cleanup
    outFile.close();
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
                    start->fileEntries.at(i) = *newEntry;

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
                        start->folderEntries.at(i).folder = *newEntry;

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
                GetFileEntry(locationOfFile, &start->folderEntries.at(i), out);
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

    toReturn.entries = new HashEntry[toReturn.entryCount];
    for  (DWORD i = 0; i < toReturn.entryCount; i++)
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
    CSHA1 sha1;
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
            // loop through all of the level0 hash blocks
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
            // iterate through all of the level1 tables
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
    if (isPEC)
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
    sha1.Update(buffer, headerSize);
    sha1.Final();
    sha1.GetHash(metaData->headerHash);
    sha1.Reset();

    delete[] buffer;

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

void StfsPackage::Resign()
{
    /*BYTE toSign[0x118];
    io->setPosition(0x22C);
    io->readBytes(toSign, 0x118);

    BYTE Modulus[128] = { 0xA3, 0x1D, 0x6C, 0xE5, 0xFA, 0x95, 0xFD, 0xE8, 0x90, 0x21, 0xFA, 0xD1, 0x0C, 0x64, 0x19, 0x2B, 0x86, 0x58, 0x9B, 0x17, 0x2B, 0x10, 0x05, 0xB8, 0xD1, 0xF8, 0x4C, 0xEF, 0x53, 0x4C, 0xD5, 0x4E, 0x5C, 0xAE, 0x86, 0xEF, 0x92, 0x7B, 0x90, 0xD1, 0xE0, 0x62, 0xFD, 0x7C, 0x54, 0x55, 0x9E, 0xE0, 0xE7, 0xBE, 0xFA, 0x3F, 0x9E, 0x15, 0x6F, 0x6C, 0x38, 0x4E, 0xAF, 0x07, 0x0C, 0x61, 0xAB, 0x51, 0x5E, 0x23, 0x53, 0x14, 0x18, 0x88, 0xCB, 0x6F, 0xCB, 0xC5, 0xD6, 0x30, 0xF4, 0x06, 0xED, 0x24, 0x23, 0xEF, 0x25, 0x6D, 0x00, 0x91, 0x77, 0x24, 0x9B, 0xE5, 0xA3, 0xC0, 0x27, 0x90, 0xC2, 0x97, 0xF7, 0x74, 0x9D, 0x6F, 0x17, 0x83, 0x7E, 0xB5, 0x37, 0xDE, 0x51, 0xE8, 0xD7, 0x1C, 0xE1, 0x56, 0xD9, 0x56, 0xC8, 0xC3, 0xC3, 0x20, 0x9D, 0x64, 0xC3, 0x2F, 0x8C, 0x91, 0x92, 0x30, 0x6F, 0xDB };
    BYTE prime1[64] = { 0xCC, 0xE7, 0x5D, 0xFE, 0x72, 0xB6, 0xFD, 0xE7, 0x1D, 0xE3, 0x1A, 0x0E, 0xAC, 0x33, 0x7A, 0xB9, 0x21, 0xE8, 0x8A, 0x84, 0x9B, 0xDA, 0x9F, 0x1E, 0x58, 0x34, 0x68, 0x7A, 0xB1, 0x1D, 0x7E, 0x1C, 0x18, 0x52, 0x65, 0x7B, 0x97, 0x8E, 0xA7, 0x6A, 0x9D, 0xEE, 0x5A, 0x77, 0x52, 0x3B, 0x71, 0x8F, 0x33, 0xD0, 0x49, 0x5E, 0xC3, 0x30, 0x39, 0x72, 0x36, 0xBF, 0x1D, 0xD9, 0xF2, 0x24, 0xE8, 0x71 };
    BYTE prime2[64] = { 0xCB, 0xCA, 0x58, 0x74, 0xD4, 0x03, 0x62, 0x93, 0x06, 0x50, 0x1F, 0x42, 0xF6, 0xAA, 0x59, 0x36, 0xA7, 0xA1, 0xF3, 0x97, 0x5C, 0x9A, 0xC8, 0x6A, 0x27, 0xCF, 0x85, 0x05, 0x2A, 0x66, 0x41, 0x6A, 0x7F, 0x2F, 0x84, 0xC8, 0x18, 0x13, 0xC6, 0x1D, 0x8D, 0xC7, 0x32, 0x2F, 0x72, 0x19, 0x3F, 0xA4, 0xED, 0x71, 0xE7, 0x61, 0xC0, 0xCF, 0x61, 0xAE, 0x8B, 0xA0, 0x68, 0xA7, 0x7D, 0x83, 0x23, 0x0B };
    BYTE publicExponent[4] = { 0x00, 0x01, 0x00, 0x01 };
    BYTE exponent1[64] = { 0x4C, 0xCA, 0x74, 0xE6, 0x74, 0x35, 0x72, 0x48, 0x58, 0x62, 0x11, 0x14, 0xE8, 0xA2, 0x4E, 0x5E, 0xED, 0x7F, 0x49, 0xD2, 0x52, 0xDA, 0x87, 0x01, 0x87, 0x4A, 0xF4, 0xD0, 0xEE, 0x69, 0xC0, 0x26, 0x65, 0x53, 0x13, 0xE7, 0x52, 0xB0, 0x4A, 0xBB, 0xE1, 0x3E, 0x3F, 0xB7, 0x32, 0x21, 0x46, 0xF8, 0xC5, 0x11, 0x4D, 0x3D, 0xEF, 0x66, 0xB6, 0x50, 0xC0, 0x85, 0xB5, 0x79, 0x45, 0x8F, 0x61, 0x71 };
    BYTE exponent2[64] = { 0xAF, 0xDC, 0x46, 0xE7, 0x52, 0x8A, 0x35, 0x47, 0xA1, 0x1C, 0x05, 0x4E, 0x39, 0x24, 0x99, 0xE6, 0x43, 0x54, 0xCB, 0xAB, 0xE3, 0xDB, 0x22, 0x76, 0x11, 0x32, 0xD0, 0x9C, 0xBB, 0x91, 0x10, 0x84, 0x81, 0x8B, 0x15, 0x2F, 0xC3, 0x2F, 0x55, 0x38, 0xED, 0xBF, 0x67, 0x3C, 0x70, 0x5E, 0xFF, 0x80, 0x28, 0xF3, 0xB1, 0x73, 0xB6, 0xFA, 0x7F, 0x56, 0x2B, 0xE1, 0xDA, 0x4E, 0x27, 0x4E, 0xC2, 0x2F };
    BYTE coefficient[64] = { 0x28, 0x6A, 0xBB, 0xD1, 0x93, 0x95, 0x94, 0x1A, 0x6E, 0xED, 0xD7, 0x0E, 0xC0, 0x61, 0x2B, 0xC2, 0xEF, 0xE1, 0x86, 0x3D, 0x34, 0x12, 0x88, 0x6F, 0x94, 0xA4, 0x48, 0x6E, 0xC9, 0x87, 0x1E, 0x46, 0x00, 0x46, 0x00, 0x52, 0x8E, 0x9F, 0x47, 0xC0, 0x8C, 0xAB, 0xBC, 0x49, 0xAC, 0x5B, 0x13, 0xF2, 0xEC, 0x27, 0x8D, 0x1B, 0x6E, 0x51, 0x06, 0xA6, 0xF1, 0x62, 0x1A, 0xEB, 0x78, 0x2E, 0x88, 0x48 };
    BYTE privateExponent[128] = { 0x51, 0xEC, 0x1F, 0x9D, 0x56, 0x26, 0xC2, 0xFC, 0x10, 0xA6, 0x67, 0x64, 0xCB, 0x3A, 0x6D, 0x4D, 0xA1, 0xE7, 0x4E, 0xA8, 0x42, 0xF0, 0xF4, 0xFD, 0xFA, 0x66, 0xEF, 0xC7, 0x8E, 0x10, 0x2F, 0xE4, 0x1C, 0xA3, 0x1D, 0xD0, 0xCE, 0x39, 0x2E, 0xC3, 0x19, 0x2D, 0xD0, 0x58, 0x74, 0x79, 0xAC, 0x08, 0xE7, 0x90, 0xC1, 0xAC, 0x2D, 0xC6, 0xEB, 0x47, 0xE8, 0x3D, 0xCF, 0x4C, 0x6D, 0xFF, 0x51, 0x65, 0xD4, 0x6E, 0xBD, 0x0F, 0x15, 0x79, 0x37, 0x95, 0xC4, 0xAF, 0x90, 0x9E, 0x2B, 0x50, 0x8A, 0x0A, 0x22, 0x4A, 0xB3, 0x41, 0xE5, 0x89, 0x80, 0x73, 0xCD, 0xFA, 0x21, 0x02, 0xF5, 0xDD, 0x30, 0xDD, 0x07, 0x2A, 0x6F, 0x34, 0x07, 0x81, 0x97, 0x7E, 0xB2, 0xFB, 0x72, 0xE9, 0xEA, 0xC1, 0x88, 0x39, 0xAC, 0x48, 0x2B, 0xA8, 0x4D, 0xFC, 0xD7, 0xED, 0x9B, 0xF9, 0xDE, 0xC2, 0x45, 0x93, 0x4C, 0x4C };

    Botan::BigInt n = Botan::BigInt::decode(Modulus, 0x80);
    Botan::BigInt e = Botan::BigInt::decode(publicExponent, 4);
    Botan::BigInt p = Botan::BigInt::decode(prime1, 0x40);
    Botan::BigInt q = Botan::BigInt::decode(prime2, 0x40);

    Botan::AutoSeeded_RNG rng;
    Botan::RSA_PrivateKey pkey(rng, p, p, 65537, 0, n);

    Botan::PK_Signer signer(pkey, "EMSA3(SHA-1)");
    Botan::SecureVector< Botan::byte > signature = signer.sign_message(toSign, 0x118, rng );

    BYTE *sig = (BYTE*)&signature[0];
    io->setPosition(0x1AC);
    io->write(sig, 0x80);*/

    throw string("Not yet implemented.\n");
}

void StfsPackage::SetBlockStatus(DWORD blockNum, BlockStatusLevelZero status)
{
    DWORD statusAddress = GetHashAddressOfBlock(blockNum) + 0x14;
    io->setPosition(statusAddress);
    io->write((BYTE)status);
}

void StfsPackage::HashBlock(BYTE *block, BYTE *outBuffer)
{
    // hash the block
    sha1.Update(block, 0x1000);
    sha1.Final();
    sha1.GetHash(outBuffer);
    sha1.Reset();
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
    length /= 8;

    for (int i = 0; i < length; i++)
    {
        for (int x = 0; x < 4; x++)
        {
            int loc = (i * 8) + x;
            int distFromCent = abs(x - 3);
            BYTE temp = data[loc];
            data[loc] = data[i * 8 + 4 + distFromCent];
            data[i * 8 + 4 + distFromCent] = temp;
        }
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

    bool alwaysAllocate = false;

    // go to the block where the file listing begins (for overwriting)
    DWORD block = metaData->volumeDescriptor.fileTableBlockNum;
    io->setPosition(BlockToAddress(block));

    DWORD outFileSize = outFolders.size();

    // add all folders to the folders map
    for (int i = 0; i < outFileSize; i++)
        folders[outFolders.at(i).entryIndex] = i;

    // write the folders to the listing
    for (int i = 0; i < outFolders.size(); i++)
    {
        // check to see if we need to go to the next block
        if ((i + 1) % 0x40 == 0)
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
        if (i % 0x40 == 0)
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
    int remainer = 0x1000 - ((outFoldersAndFilesSize % 0x40) * 0x40);
    BYTE *nullBytes = new BYTE[remainer];
    memset(nullBytes, 0, remainer);
    io->write(nullBytes, remainer);

    // update the file table block count and write it to file
    metaData->volumeDescriptor.fileTableBlockCount = ((outFiles.size() + outFileSize) * 0x40 / 0x1000) + 1;
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
    io->write(entry->updateTimeStamp);
    io->write(entry->accessTimeStamp);
}

void StfsPackage::RemoveFile(string pathInPackage)
{
    RemoveFile(GetFileEntry(pathInPackage));
}

INT24 StfsPackage::AllocateBlock()
{
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
        }
    } //vite

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

    // terminate the chain
    io->write((INT24)0xFFFFFF);

    metaData->WriteVolumeDescriptor();
    return metaData->volumeDescriptor.allocatedBlockCount - 1;
}

void StfsPackage::FindDirectoryListing(vector<string> locationOfDirectory, FileListing *start, FileListing **out)
{
    if(locationOfDirectory.size() == 0)
        *out = start;

    bool finalLoop = (locationOfDirectory.size() == 1);
    for (int i = 0; i < start->folderEntries.size(); i++)
    {
        if (start->folderEntries.at(i).folder.name == locationOfDirectory.at(0))
        {
            locationOfDirectory.erase(locationOfDirectory.begin());
            if (finalLoop)
                *out = &start->folderEntries.at(i);
            else
                for (int i = 0; i < start->folderEntries.size(); i++)
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

void StfsPackage::InjectFile(string path, string pathInPackage)
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
    entry.fileSize = fileSize;
    entry.flags = 0;
    entry.pathIndicator = folder->folder.entryIndex;
    entry.startingBlockNum = -1;
    entry.blocksForFile = ((fileSize + 0xFFF) & 0xFFFFFFF000) >> 0xC;

    INT24 block, prevBlock = -1;
    while(fileSize >= 0x1000)
    {
        block = AllocateBlock();

        if (entry.startingBlockNum == -1)
            entry.startingBlockNum = block;

        if (prevBlock != -1)
            SetNextBlock(prevBlock, block);

        prevBlock = block;

        // read the data
        BYTE data[0x1000];
        fileIn.readBytes(data, 0x1000);

        io->setPosition(BlockToAddress(block));
        io->write(data, 0x1000);

        fileSize -= 0x1000;
    }

    if(fileSize != 0)
    {
        block = AllocateBlock();

        if (entry.startingBlockNum == -1)
            entry.startingBlockNum = block;

        if (prevBlock != -1)
            SetNextBlock(prevBlock, block);

        BYTE *data = new BYTE[fileSize];
        fileIn.readBytes(data, fileSize);
        io->setPosition(BlockToAddress(block));
        io->write(data, fileSize);

        fileSize = 0;
    }

    SetNextBlock(block, INT24_MAX);

    folder->fileEntries.push_back(entry);
    WriteFileListing();


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

void StfsPackage::ReplaceFile(string path, string pathInPackage)
{
    FileEntry entry = GetFileEntry(pathInPackage);

    if (entry.nameLen == 0)
        throw string("STFS: File doesn't exists in the package.\n");

    FileIO fileIn(path);

    fileIn.setPosition(0, ios_base::end);
    DWORD fileSize = fileIn.getPosition();
    fileIn.setPosition(0);

    // set up the entry
    entry.fileSize = fileSize;
    entry.blocksForFile = ((fileSize + 0xFFF) & 0xFFFFFFF000) >> 0xC;

    DWORD block = entry.startingBlockNum;
    io->setPosition(BlockToAddress(block));

    DWORD fullReads = fileSize / 0x1000;
    bool first = true, alwaysAllocate = false;

    // write the folders to the listing
    for (int i = 0; i < fullReads; i++)
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
    }

    SetNextBlock(block, INT24_MAX);

    entry.flags &= 0x2;

    io->setPosition(entry.fileEntryAddress + 0x28);
    io->write((BYTE)(entry.nameLen | (entry.flags << 6)));
    io->write(entry.blocksForFile, LittleEndian);
    io->write(entry.blocksForFile, LittleEndian);

    io->setPosition(entry.fileEntryAddress + 0x34);
    io->write(entry.fileSize);
    UpdateEntry(pathInPackage, entry);

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
}
