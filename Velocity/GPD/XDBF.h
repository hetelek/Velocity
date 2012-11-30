#pragma once
#include <iostream>
#include <vector>
#include "../FileIO.h"
#include "XDBFDefininitions.h"
#include "XDBFHelpers.h"
#include <algorithm>

using std::string;
using std::vector;

class XDBF
{
public:
    XDBF(string gpdPath);
    XDBF(FileIO *io);
    ~XDBF();

    XDBFEntryGroup achievements;
    vector<XDBFEntry> images;
    XDBFEntryGroup settings;
    XDBFEntryGroup titlesPlayed;
    vector<XDBFEntry> strings;
    XDBFEntryGroup avatarAwards;

    // Description: extract the bytes of a given entry to the out buffer
    void ExtractEntry(XDBFEntry entry, BYTE *outBuffer);

    // Description: delete an entry from the file
    void DeleteEntry(XDBFEntry entry);

    // Description: convert a specifier address into a real address
    DWORD GetRealAddress(DWORD specifier);

    // Description: convert a real address into a specifer
    DWORD GetSpecifier(DWORD address);

    // Description: create a new entry and new sync in the file and return the entry
    XDBFEntry CreateEntry(EntryType type, UINT64 id, DWORD length);

    // Description: allocate memory in the file
    DWORD AllocateMemory(DWORD size);

    // Description: free memory in the file at the specifer address
    void DeallocateMemory(DWORD addr, DWORD size);

    // Description: move the sync to the queue
    void UpdateEntry(XDBFEntry *entry);

    // Description: remove all the unused memory in the file
    void Clean();

    // Description: re-write an entry
    void RewriteEntry(XDBFEntry entry, BYTE *entryBuffer);
    FileIO *io;

private:
    bool ioPassedIn;
    XDBFHeader header;
    vector<XDBFFreeMemEntry> freeMemory;

    // Description: read in the XDBF header
    void readHeader();

    // Description: read in the file table
    void readEntryTable();

    // Description: read the free memory table
    void readFreeMemoryTable();

    // Description: re-write the entry listing
    void writeEntryListing();

    // Description: re-write the free memory table
    void writeFreeMemTable();

    // Description: re-write the header
    void writeHeader();

    // Description: read a sync list based off the entry passed in
    SyncList readSyncList(XDBFEntry entry);

    // Description: write the sync list back to the file
    void writeSyncList(SyncList *syncs);

    // Description: read sync data based off the entry passed in
    SyncData readSyncData(XDBFEntry entry);

    // Description: write the sync data
    void writeSyncData(SyncData *data);

    // Description: read an entry group from the table that has syncs
    void readEntryGroup(XDBFEntryGroup *group, EntryType type);

    // Description: read an entry group from the table that doesn't have syncs
    void readEntryGroup(vector<XDBFEntry> *group, EntryType type);

    // Description: write an entry group to the table that has syncs
    void writeEntryGroup(XDBFEntryGroup *group);

    // Description: write an entry group to the table that doesn't have syncs
    void writeEntryGroup(vector<XDBFEntry> *group);

    // Description: null out the structs that need it
    void init();

    // Description: update the final entry in the free memory, stores the file length
    void updateFreeMemTable();

    // Description: write an entry to the entry table
    void writeEntry(XDBFEntry *entry);

    // Description: write an entry group to the file that has syncs, used when cleaning
    void writeNewEntryGroup(XDBFEntryGroup *group, FileIO *newIO);

    // Description: write an entry group that doesn't have syncs, used when cleaning
    void writeNewEntryGroup(vector<XDBFEntry> *group, FileIO *newIO);

    // Description: write entry to the new file, used when cleaing
    void writeNewEntry(XDBFEntry *entry, FileIO *newIO);
};

bool compareEntries(XDBFEntry a, XDBFEntry b);
