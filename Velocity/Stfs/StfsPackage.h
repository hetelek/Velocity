#pragma once

#include <iostream>
#include <sstream>
#include <math.h>
#include <map>
#include <time.h>
#include <stdlib.h>
#include "../FileIO.h"
#include "StfsMetaData.h"

#include <botan/botan.h>
#include <botan/pubkey.h>
#include <botan/rsa.h>
#include <botan/emsa.h>
#include <botan/sha160.h>
#include <botan/emsa3.h>
#include <botan/look_pk.h>

#include <QVector>

#include <QString>

using std::stringstream;

struct FileEntry
{
    DWORD entryIndex;
    QString name;
    BYTE nameLen;
    BYTE flags;
    INT24 blocksForFile;
    INT24 startingBlockNum;
    WORD pathIndicator;
    DWORD fileSize;
    DWORD createdTimeStamp;
    DWORD accessTimeStamp;
    DWORD fileEntryAddress;
};

struct FileListing
{
    QVector<FileEntry> fileEntries;
    QVector<FileListing> folderEntries;
    FileEntry folder;
};

#pragma pack(push, 1)
struct HashEntry
{
    BYTE blockHash[0x14];
    BYTE status;
    DWORD nextBlock;
};
#pragma pack(pop)

struct HashTable
{
    Level level;
    DWORD trueBlockNumber;
    DWORD entryCount;
    HashEntry entries[0xAA];
    DWORD addressInFile;
};

enum StfsPackageFlags
{
    StfsPackagePEC = 1,
    StfsPackageCreate = 2,
    StfsPackageFemale = 4     // only used when creating a packge
};

class StfsPackage
{
public:
    StfsMetaData *metaData;

    // Description: initialize a stfs package
    StfsPackage(const QString &packagePath, DWORD flags = 0);

    // Description: get the file listing of the package, forceUpdate reads from the package regardless
    FileListing GetFileListing(bool forceUpdate = false);

    // Description: extract a file to designated file path
    void ExtractFile(const QString &pathInPackage, const QString &outPath, void(*extractProgress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // Description: extract a file (by FileEntry) to a designated file path
    void ExtractFile(FileEntry *entry, const QString &outPath, void(*extractProgress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // Description: get the file entry of a file's path, sets nameLen to '0' if not found
    FileEntry GetFileEntry(const QString &pathInPackage, bool checkFolders = false, FileEntry *newEntry = NULL);

    // Description: get the first 4 bytes of a file
    DWORD GetFileMagic(const QString &pathInPackage);

    // Description: check if the file exists
    bool FileExists(const QString &pathInPackage);

    // Description: fix all the hashes used in the file
    void Rehash();

    // Description: resign the file
    void Resign(const QString &kvPath);

    // Description: remove a file entry from the file listing
    void RemoveFile(FileEntry entry);

    // Description: remove a file entry from the file listing
    void RemoveFile(const QString &pathInPackage);

    // Description: inject a file into the package
    FileEntry InjectFile(const QString &path, const QString &pathInPackage, void(*injectProgress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // Description: inject raw data into the package
    FileEntry InjectData(BYTE *data, DWORD length, const QString &pathInPackage, void(*injectProgress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // Description: replace an existing file into the package
    void ReplaceFile(const QString &path, const QString &pathInPackage, void(*replaceProgress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // Description: replace an existing file into the package
    void ReplaceFile(const QString &path, FileEntry *entry, const QString &pathInPackage, void(*replaceProgress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // Description: rename an existing file in the package
    void RenameFile(const QString &newName, const QString &pathInPackage);

    // Description: convert a block into an address in the file
    DWORD BlockToAddress(DWORD blockNum) const;

    // Description: get the address of a hash for a data block
    DWORD GetHashAddressOfBlock(DWORD blockNum) const;

    // Description: returns whether the 'isPEC' parameter is set
    bool IsPEC() const;

    // Description: close the io and all other resources used
    void Close();

    ~StfsPackage(void);
private:
    FileListing fileListing;
    FileListing writtenToFile;

    FileIO *io;
    stringstream except;

    Sex packageSex;
    DWORD blockStep[2];
    DWORD firstHashTableAddress;
    BYTE hashOffset;
    Level topLevel;
    HashTable topTable;
    HashTable cached;
    DWORD tablesPerLevel[3];

    DWORD flags;

    // Description: read the file listing from the file
    void ReadFileListing();

    // Description: set the out buffer to the sha1 of the block
    void HashBlock(BYTE *block, BYTE *outBuffer);

    // Description: swap the table used so there is a backup of the data modified
    void SwapTable(DWORD index, Level lvl);

    // Description: extract a block's data
    void ExtractBlock(DWORD blockNum, BYTE *data, DWORD length = 0x1000);

    // Description: convert a block number into a true block number, where the first block is the first hash table
    DWORD ComputeBackingDataBlockNumber(DWORD blockNum) const;

    // Description: get a block's hash entry
    HashEntry GetBlockHashEntry(DWORD blockNum) const;

    // Description: get the true block number for the hash table that hashes the block at the level passed in
    DWORD ComputeLevelNBackingHashBlockNumber(DWORD blockNum, Level level) const;

    // Description: get the true block number for the hash table that hashes the block at level 0
    DWORD ComputeLevel0BackingHashBlockNumber(DWORD blockNum) const;

    // Description: get the true block number for the hash table that hashes the block at level 1 (female)
    DWORD ComputeLevel1BackingHashBlockNumber(DWORD blockNum) const;

    // Description: get the true block number for the hash table that hashes the block at level 2
    DWORD ComputeLevel2BackingHashBlockNumber(DWORD blockNum) const;

    // Descrption: get the address of a hash table in the package
    DWORD GetHashTableAddress(DWORD index, Level lvl);

    // Description: get the base address of a hash table in the package
    DWORD GetBaseHashTableAddress(DWORD index, Level lvl);

    // Description: get the amount of hash entries in a hash table
    DWORD GetHashTableEntryCount(DWORD index, Level lvl);

    // Description: get the hash table at the current index on the specified level
    HashTable GetLevelNHashTable(DWORD index, Level lvl);

    // Description: build the table in memory for preperation to write
    void BuildTableInMemory(HashTable *table, BYTE *outBuffer);

    // Description: add the file entry to the file listing
    void AddToListing(FileListing *fullListing, FileListing *out);

    // Description: get the file entry from a file's path
    void GetFileEntry(QVector<QString> locationOfFile, FileListing *start, FileEntry *out, FileEntry *newEntry = NULL, bool updateEntry = false, bool checkFolders = false);

    // Description: get the raw file listing
    void GenerateRawFileListing(FileListing *in, QVector<FileEntry> *outFiles, QVector<FileEntry> *outFolders);

    // Description: split a string into multiple substrings
    QVector<QString> SplitString(const QString &str, const QString &delimeter);

    // Description: get the hash address in the parent hash table to the corresponding table
    DWORD GetTableHashAddress(DWORD index, Level lvl);

    // Description: set the status of the block in the corresponding hash table
    void SetBlockStatus(DWORD blockNum, BlockStatusLevelZero status);

    // Description: set the next block of the current block
    void SetNextBlock(DWORD blockNum, INT24 nextBlockNum);

    // Description: discard the current file listing and rewrite it
    void WriteFileListing(bool usePassed = false, QVector<FileEntry> *outFis = NULL, QVector<FileEntry> *outFos = NULL);

    // Description: write a file entry at the io's current position
    void WriteFileEntry(FileEntry *entry);

    // Description: allocate a data block in the package, and return a block number
    INT24 AllocateBlock();

    // Description: allocate 'blockCount' consecutive data blocks
    INT24 AllocateBlocks(DWORD blockCount);

    // Description: calculate the level of the topmost hash table
    Level CalcualateTopLevel();

    // Description: get the file listing that matches the directory path
    void FindDirectoryListing(QVector<QString> locationOfDirectory, FileListing *start, FileListing **out);

    // Description: update the entry at the given path
    void UpdateEntry(const QString &pathInPackage, FileEntry entry);

    // Description: swap bytes by chunks of 8
    void XeCryptBnQw_SwapDwQwLeBe(BYTE *data, DWORD length);

    // Description: get the number of bytes to skip over the hash table
    DWORD GetHashTableSkipSize(DWORD tableAddress);

    // Description: get the number of blocks until the next hash table
    DWORD GetBlocksUntilNextHashTable(DWORD currentBlock);

    // Description: parse the file
    void Parse();
};
