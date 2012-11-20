#include "XDBF.h"
#include <stdio.h>

XDBF::XDBF(string gpdPath) : ioPassedIn(false)
{
    io = new FileIO(gpdPath);

    init();
    readHeader();
    readEntryTable();
    readFreeMemoryTable();
}

XDBF::XDBF(FileIO *io) : ioPassedIn(true)
{
    this->io = io;

    init();
    readHeader();
    readEntryTable();
    readFreeMemoryTable();
}

void XDBF::Clean()
{
    // create a temporary file to write the old GPD's used memory to
    char *tempFileName = tmpnam(NULL);
    FileIO tempFile(string((char*)tempFileName), true);

    // write the old header
    tempFile.setPosition(0);
    tempFile.write(header.magic);
    tempFile.write(header.version);
    tempFile.write(header.entryTableLength);
    tempFile.write(header.entryCount);
    tempFile.write(header.freeMemTableLength);
    tempFile.write((DWORD)1);

    // write the free mem table value
    tempFile.setPosition((header.entryTableLength * 0x12) + 0x1C);
    tempFile.write((DWORD)0xFFFFFFFF);

    // seek to the first position in the file where data can be written
    tempFile.setPosition(GetRealAddress(0) - 1);
    tempFile.write((BYTE)0);

    tempFile.flush();

    // write all of the achievements
    writeNewEntryGroup(&achievements, &tempFile);

    // write all of the images
    writeNewEntryGroup(&images, &tempFile);

    // write all of the settings
    writeNewEntryGroup(&settings, &tempFile);

    // write all of the title entries
    writeNewEntryGroup(&titlesPlayed, &tempFile);

    // write all of the strings
    writeNewEntryGroup(&strings, &tempFile);

    // write all of the achievements
    writeNewEntryGroup(&avatarAwards, &tempFile);

    tempFile.close();
    io->close();

    // delete the original file
    remove(io->getFilePath().c_str());

    // move the temp file to the old file's location
    rename(tempFileName, io->getFilePath().c_str());

    string path = io->getFilePath();
    delete io;
    io = new FileIO(path);

    // write the updated entry table
    writeEntryListing();

    // clear all of the existing entries
    achievements.entries.clear();
    images.clear();
    settings.entries.clear();
    titlesPlayed.entries.clear();
    strings.clear();
    avatarAwards.entries.clear();

    // reload the file listing
    readEntryTable();
}

void XDBF::writeNewEntryGroup(XDBFEntryGroup *group, FileIO *newIO)
{
    // iterate through all of the entries
    for (DWORD i = 0; i < group->entries.size(); i++)
        writeNewEntry(&group->entries.at(i), newIO);

    // write the sync crap
    writeNewEntry(&group->syncs.entry, newIO);
    writeNewEntry(&group->syncData.entry, newIO);
}

void XDBF::writeNewEntryGroup(vector<XDBFEntry> *group, FileIO *newIO)
{
    // iterate through all of the entries
    for (DWORD i = 0; i < group->size(); i++)
        writeNewEntry(&group->at(i), newIO);
}

void XDBF::writeNewEntry(XDBFEntry *entry, FileIO *newIO)
{
    // read in the entry data
    BYTE *buffer = new BYTE[entry->length];
    io->setPosition(GetRealAddress(entry->addressSpecifier));
    io->readBytes(buffer, entry->length);

    // update the entry address
    entry->addressSpecifier = GetSpecifier((DWORD)newIO->getPosition());

    // write the entry data
    newIO->write(buffer, entry->length);

    delete[] buffer;
}

void XDBF::init()
{
    achievements.syncs.lengthChanged = false;
    settings.syncs.lengthChanged = false;
    titlesPlayed.syncs.lengthChanged = false;
    avatarAwards.syncs.lengthChanged = false;

    memset(&achievements.syncs.entry, 0, sizeof(XDBFEntry));
    memset(&settings.syncs.entry, 0, sizeof(XDBFEntry));
    memset(&titlesPlayed.syncs.entry, 0, sizeof(XDBFEntry));
    memset(&avatarAwards.syncs.entry, 0, sizeof(XDBFEntry));

    memset(&achievements.syncData.entry, 0, sizeof(XDBFEntry));
    memset(&settings.syncData.entry, 0, sizeof(XDBFEntry));
    memset(&titlesPlayed.syncData.entry, 0, sizeof(XDBFEntry));
    memset(&avatarAwards.syncData.entry, 0, sizeof(XDBFEntry));
}

XDBF::~XDBF()
{
    if (!ioPassedIn)
        io->close();
}

void XDBF::readHeader()
{
    // seek to the begining of the file
    io->setPosition(0);

    // ensure the magic is correct
    header.magic = io->readDword();
    if (header.magic != 0x58444246)
        throw string("XDBF: Invalid magic.\n");

    // read in the rest of the header
    header.version = io->readDword();
    header.entryTableLength = io->readDword();
    header.entryCount = io->readDword();
    header.freeMemTableLength = io->readDword();
    header.freeMemTableEntryCount = io->readDword();
}

void XDBF::readEntryTable()
{
    // seek to the begining of the entry table
    io->setPosition(0x18);

    // read achievement table entries
    readEntryGroup(&achievements, Achievement);

    // read images
    readEntryGroup(&images, Image);

    // read setting table entries
    readEntryGroup(&settings, Setting);

    // read title table entries
    readEntryGroup(&titlesPlayed, Title);

    // read strings
    readEntryGroup(&strings, String);

    // read avatar award table entries
    readEntryGroup(&avatarAwards, AvatarAward);
}

SyncData XDBF::readSyncData(XDBFEntry entry)
{
    // make sure the entry passed in is sync data
    if (entry.type == Image || entry.type == String || (entry.id != 0x200000000 && entry.id != 2))
        throw string("XDBF: Error reading sync list. Specified entry isn't a sync list.\n");

    // preserve io position
    DWORD pos = (DWORD)io->getPosition();

    SyncData data;
    data.entry = entry;

    // seek to the sync data entry in the file
    io->setPosition(GetRealAddress(entry.addressSpecifier));

    // read the data
    data.nextSyncID = io->readUInt64();
    data.lastSyncID = io->readUInt64();

    FILETIME time = { io->readDword(), io->readDword() };
    data.lastSyncedTime = (tm*)XDBFHelpers::FILETIMEtoTimeT(time);

    io->setPosition(pos);

    return data;
}

void XDBF::writeSyncData(SyncData *data)
{
    // seek to the sync data position
    io->setPosition(GetRealAddress(data->entry.addressSpecifier));

    io->write(data->nextSyncID);
    io->write(data->lastSyncID);

    // write the last time synced
    FILETIME lastSynced = XDBFHelpers::TimeTtoFILETIME((time_t)data->lastSyncedTime);
    io->write(lastSynced.dwHighDateTime);
    io->write(lastSynced.dwLowDateTime);
}

SyncList XDBF::readSyncList(XDBFEntry entry)
{
    // make sure the entry passed in is a sync list
    if (entry.type == Image || entry.type == String || (entry.id != 0x100000000 && entry.id != 1))
        throw string("XDBF: Error reading sync list. Specified entry isn't a sync list.\n");

    // preserve io position
    DWORD pos = (DWORD)io->getPosition();

    SyncList toReturn;
    toReturn.entry = entry;

    // seek to the begining of the entry
    io->setPosition(GetRealAddress(entry.addressSpecifier));

    // iterate through all the syncs
    for (DWORD i = 0; i < (entry.length / 16); i++)
    {
        // read in the sync
        SyncEntry sync = { io->readUInt64(), io->readUInt64() };

        // if the sync value is 0, then it was already synced
        if (sync.syncValue == 0)
            toReturn.synced.push_back(sync);
        else
            toReturn.toSync.push_back(sync);
    }

    io->setPosition(pos);

    return toReturn;
}

void XDBF::writeSyncList(SyncList *syncs)
{

    // if the length has changed, then we need to allocated more/less memory
    if (syncs->lengthChanged)
    {
        // free the old memory
        DeallocateMemory(GetRealAddress(syncs->entry.addressSpecifier), syncs->entry.length);

        // update entry length
        syncs->entry.length = (syncs->synced.size() + syncs->toSync.size()) * 0x10;

        // allocate new memory
        syncs->entry.addressSpecifier = GetSpecifier(AllocateMemory(syncs->entry.length));
    }

    // seek to the sync list position
    io->setPosition(GetRealAddress(syncs->entry.addressSpecifier));

    // write the synced ones
    for (DWORD i = 0; i < syncs->synced.size(); i++)
    {
        io->write(syncs->synced.at(i).entryID);
        io->write(syncs->synced.at(i).syncValue);
    }

    // write the toSync ones
    for (int i = (int)syncs->toSync.size() - 1; i >= 0; i--)
    {
        io->write(syncs->toSync.at(i).entryID);
        io->write(syncs->toSync.at(i).syncValue);
    }
}

void XDBF::readFreeMemoryTable()
{
    // seek to the begining of the free memory table
    DWORD tableStartAddr = 0x18 + (header.entryTableLength * 0x12);
    io->setPosition(tableStartAddr);

    // iterate through all of the free memory table entries
    for (DWORD i = 0; i < header.freeMemTableEntryCount; i++)
    {
        XDBFFreeMemEntry entry = { io->readDword(), io->readDword() };
        freeMemory.push_back(entry);
    }
}

DWORD XDBF::GetRealAddress(DWORD specifier)
{
    return specifier + (header.entryTableLength * 0x12) + (header.freeMemTableLength * 8) + 0x18;
}

DWORD XDBF::GetSpecifier(DWORD address)
{
    DWORD headerSize = (header.entryTableLength * 0x12) + (header.freeMemTableLength * 8) + 0x18;
    if (address < headerSize)
        throw string("XDBF: Invalid address for converting.\n");
    return address - headerSize;
}

XDBFEntry XDBF::CreateEntry(EntryType type, UINT64 id, DWORD size)
{
    XDBFEntry entry = { type, id, 0, size };

    header.entryCount++;

    if (id == ((type == AvatarAward) ? 1 : 0x100000000) || id == ((type == AvatarAward) ? 2 : 0x200000000))
    {
        // allocate memory for the entry
        entry.addressSpecifier = GetSpecifier(AllocateMemory(size));
        writeEntryListing();

        writeHeader();

        return entry;
    }

    // make sure the entry doesn't already exist
    vector<XDBFEntry> *entries;
    switch (entry.type)
    {
        case Achievement:
            entries = &achievements.entries;
            break;
        case Image:
            entries = &images;
            break;
        case Setting:
            entries = &settings.entries;
            break;
        case Title:
            entries = &titlesPlayed.entries;
            break;
        case String:
            entries = &strings;
            break;
        case AvatarAward:
            entries = &avatarAwards.entries;
            break;
        default:
            throw string("XDBF: Error creating entry. Invalid entry type.\n");
    }
    for (DWORD i = 0; i < entries->size(); i++)
        if (entries->at(i).id == entry.id)
            throw string("XDBF: Error creating entry. Entry already exists.\n");

    // allocate memory for the entry
    entry.addressSpecifier = GetSpecifier(AllocateMemory(size));

    // add the entry to the listing, and create a new sync for the entry if needed
    switch (entry.type)
    {
        case Achievement:
        {
            // add the entry to the list
            achievements.entries.push_back(entry);
            // create a sync for the entry
            SyncEntry sync = { entry.id, achievements.syncData.nextSyncID++ };
            achievements.syncs.toSync.push_back(sync);
            achievements.syncs.lengthChanged = true;
            // re-write the sync list
            writeSyncList(&achievements.syncs);
            writeSyncData(&achievements.syncData);
            break;
        }
        case Image:
            images.push_back(entry);
            break;
        case Setting:
        {
            // add the entry to the list
            settings.entries.push_back(entry);
            // create a sync for the entry
            SyncEntry sync = { entry.id, settings.syncData.nextSyncID++ };
            settings.syncs.toSync.push_back(sync);
            // re-write the sync list
            writeSyncList(&settings.syncs);
            writeSyncData(&settings.syncData);
            break;
        }
        case Title:
        {
            // add the entry to the list
            titlesPlayed.entries.push_back(entry);
            // create a sync for the entry
            SyncEntry sync = { entry.id, titlesPlayed.syncData.nextSyncID++ };
            titlesPlayed.syncs.toSync.push_back(sync);
            // re-write the sync list
            writeSyncList(&titlesPlayed.syncs);
            writeSyncData(&titlesPlayed.syncData);
            break;
        }
        case String:
            strings.push_back(entry);
            break;
        case AvatarAward:
        {
            // add the entry to the list
            avatarAwards.entries.push_back(entry);
            // create a sync for the entry
            SyncEntry sync = { entry.id, avatarAwards.syncData.nextSyncID++ };
            avatarAwards.syncs.toSync.push_back(sync);
            // re-write the sync list
            writeSyncList(&avatarAwards.syncs);
            writeSyncData(&avatarAwards.syncData);
            break;
        }
    }

    writeEntryListing();

    writeHeader();

    return entry;
}

DWORD XDBF::AllocateMemory(DWORD size)
{
    if (size == 0)
        return GetRealAddress(0);

    DWORD toReturn;

    // first checek and see if we can allocate some of the memory in the free memory table
    int index = 0;
    for (; index < (int)(freeMemory.size() - 1); index++)
        if (freeMemory.at(index).length >= size)
            break;
    // if the memory wasn't found in the table, then we need to append it to the file
    if (index == (freeMemory.size() - 1))
    {
        // get the position in the file of the memory allocated
        io->setPosition(0, ios_base::end);
        toReturn = (DWORD)io->getPosition();

        io->flush();
        io->setPosition(size - 1, ios_base::end);
        io->write((BYTE)0);
        io->flush();
    }
    else
    {
        // get the the real address of the free memory in the file
        toReturn = GetRealAddress(freeMemory.at(index).addressSpecifier);

        // erase the entry used from the vector
        XDBFFreeMemEntry temp = freeMemory.at(index);
        freeMemory.erase(freeMemory.begin() + index);

        // update the header
        header.freeMemTableEntryCount--;

        // if there is more memory there than needed, then give some back
        if (size != temp.length)
            DeallocateMemory(GetRealAddress(temp.addressSpecifier + size), temp.length - size);

        // update what needs to be updated
        writeFreeMemTable();
        writeHeader();
    }

    return toReturn;
}

void XDBF::DeallocateMemory(DWORD addr, DWORD size)
{
    if (size == 0)
        return;

    // create a new free mem table entry
    XDBFFreeMemEntry entry;
    entry.addressSpecifier = GetSpecifier(addr);
    entry.length = size;

    // add the new entry to the table
    freeMemory.insert(freeMemory.begin() + (freeMemory.size() - 1), entry);

    // update the header
    header.freeMemTableEntryCount++;

    // update the table
    writeFreeMemTable();
}

void XDBF::readEntryGroup(XDBFEntryGroup *group, EntryType type)
{
    XDBFEntry entry = { (EntryType)io->readInt16(), io->readUInt64(), io->readDword(), io->readDword() };
    group->syncData.entry.type = (EntryType)0;
    group->syncs.entry.type = (EntryType)0;

    while (entry.type == type)
    {
        if (entry.id == ((type == AvatarAward) ? 1 : 0x100000000))
            group->syncs = readSyncList(entry);
        else if (entry.id == ((type == AvatarAward) ? 2 : 0x200000000))
            group->syncData = readSyncData(entry);
        else
            group->entries.push_back(entry);

        entry.type = (EntryType)io->readInt16();
        entry.id = io->readUInt64();
        entry.addressSpecifier = io->readDword();
        entry.length = io->readDword();
    }

    // back the io up 1 entry
    io->setPosition((DWORD)io->getPosition() - 0x12);
}

void XDBF::readEntryGroup(vector<XDBFEntry> *group, EntryType type)
{
    XDBFEntry entry;

    while (true)
    {
        entry.type = (EntryType)io->readInt16();
        entry.id = io->readUInt64();
        entry.addressSpecifier = io->readDword();
        entry.length = io->readDword();

        if (entry.type != type)
            break;
        group->push_back(entry);
    }

    // back the io up 1 entry
    io->setPosition((DWORD)io->getPosition() - 0x12);
}

void XDBF::writeEntryListing()
{
    // seek to the begining of the entry table
    io->setPosition(0x18);

    // clear the current table
    BYTE buffer[0x12] = {0};
    for (DWORD i = 0; i < header.entryTableLength; i++)
        io->write(buffer, 0x12);
    io->setPosition(0x18);
    io->flush();

    // write achievement entries
    writeEntryGroup(&achievements);
    io->flush();

    // write image entries
    writeEntryGroup(&images);
    io->flush();

    // write setting entries
    writeEntryGroup(&settings);
    io->flush();

    // write title entries
    writeEntryGroup(&titlesPlayed);
    io->flush();

    // write string entries
    writeEntryGroup(&strings);
    io->flush();

    // write avatar award entries
    writeEntryGroup(&avatarAwards);
    io->flush();

    // just to be safe i guess
    io->flush();
}

void XDBF::writeFreeMemTable()
{
    // update the last free memory table entry
    io->setPosition(0, ios_base::end);
    DWORD temp = GetSpecifier(io->getPosition());
    freeMemory.at(freeMemory.size() - 1).addressSpecifier = temp;
    freeMemory.at(freeMemory.size() - 1).length = (0xFFFFFFFF - temp);

    // seek to the free memory table position
    io->setPosition(0x18 + (header.entryTableLength * 0x12));

    // write the table
    for (DWORD i = 0; i < freeMemory.size(); i++)
    {
        io->write(freeMemory.at(i).addressSpecifier);
        io->write(freeMemory.at(i).length);
    }

    // null out the rest of the table
    for (DWORD i = 0; i < (header.freeMemTableLength - freeMemory.size()); i++)
        io->write((UINT64)0);
}

void XDBF::writeHeader()
{
    io->setPosition(0);
    io->write(header.magic);
    io->write(header.version);
    io->write(header.entryTableLength);
    io->write(header.entryCount);
    io->write(header.freeMemTableLength);
    io->write(header.freeMemTableEntryCount);
}

void XDBF::writeEntryGroup(XDBFEntryGroup *group)
{
    std::sort(group->entries.begin(), group->entries.end(), compareEntries);

    if (group->syncs.entry.type == AvatarAward)
    {
        // write the sync stuffs
        if (group->syncs.entry.type != 0)
            writeEntry(&group->syncs.entry);
        if (group->syncData.entry.type != 0)
            writeEntry(&group->syncData.entry);

        // write all the entries
        for (DWORD i = 0; i < group->entries.size(); i++)
            writeEntry(&group->entries.at(i));
    }
    else
    {
        // write all the entries
        for (DWORD i = 0; i < group->entries.size(); i++)
            writeEntry(&group->entries.at(i));

        // write the sync stuffs
        if (group->syncs.entry.type != 0)
            writeEntry(&group->syncs.entry);
        if (group->syncData.entry.type != 0)
            writeEntry(&group->syncData.entry);
    }
}

void XDBF::writeEntryGroup(vector<XDBFEntry> *group)
{
    std::sort(group->begin(), group->end(), compareEntries);

    // write all the entries
    for (DWORD i = 0; i < group->size(); i++)
        writeEntry(&group->at(i));
}

void XDBF::writeEntry(XDBFEntry *entry)
{
    io->write((INT16)entry->type);
    io->write((UINT64)entry->id);
    io->write((DWORD)entry->addressSpecifier);
    io->write((DWORD)entry->length);
}

void XDBF::UpdateEntry(XDBFEntry *entry)
{
    // neither of these entry types have syncs
    if (entry->type == Image || entry->type == String)
        return;

    XDBFEntryGroup *group;
    switch (entry->type)
    {
        case Achievement:
            group = &achievements;
            break;
        case Setting:
            group = &settings;
            break;
        case Title:
            group = &titlesPlayed;
            break;
        case AvatarAward:
            group = &avatarAwards;
            break;
        default:
            throw string("XDBF: Error updating entry. Invalid entry type.\n");
    }

    // find the entry in the table and update it
    for (DWORD i = 0; i < group->entries.size(); i++)
    {
        if (group->entries.at(i).id == entry->id)
        {
            group->entries.at(i).addressSpecifier = entry->addressSpecifier;
            group->entries.at(i).length = entry->length;
        }
    }

    // if the sync data doesn't exist, then create it
    if (group->syncData.entry.type == 0)
    {
        group->syncData.entry = CreateEntry(entry->type, (entry->type == AvatarAward) ? 2 : 0x200000000, 0x18);
        group->syncData.lastSyncedTime = NULL;
        group->syncData.lastSyncID = 0;
        group->syncData.nextSyncID = 1;
    }

    // if the sync list doesn't exist, then create it
    if (group->syncs.entry.type == 0)
    {
        group->syncs.entry = CreateEntry(entry->type, (entry->type == AvatarAward) ? 1 : 0x100000000, 0);
        group->syncs.lengthChanged = true;
    }

    // find the sync if it isn't already in the queue
    for (DWORD i = 0; i < group->syncs.synced.size(); i++)
    {
        if (group->syncs.synced.at(i).entryID == entry->id)
        {
            SyncEntry sync = group->syncs.synced.at(i);

            // erase it from the synced list
            group->syncs.synced.erase(group->syncs.synced.begin() + i);

            // add the sync to the queue
            sync.syncValue = group->syncData.nextSyncID++;
            group->syncs.toSync.push_back(sync);
        }
    }

    // re-write the new sync list
    writeSyncList(&group->syncs);

    // re-write the updated sync data
    writeSyncData(&group->syncData);

    writeEntryListing();
}

void XDBF::RewriteEntry(XDBFEntry entry, BYTE *entryBuffer)
{
    // get the entry list
    vector<XDBFEntry> *entryList;
    switch (entry.type)
    {
        case Achievement:
            entryList = &achievements.entries;
            break;
        case Image:
            entryList = &images;
            break;
        case Setting:
            entryList = &settings.entries;
            break;
        case Title:
            entryList = &titlesPlayed.entries;
            break;
        case String:
            entryList = &strings;
            break;
        case AvatarAward:
            entryList = &avatarAwards.entries;
            break;
        default:
            throw string("XDBF: Error rewriting entry, entry type not supported.\n");
    }

    // make sure the entry already exists
    DWORD i;
    for (i = 0; i < entryList->size(); i++)
        if (entryList->at(i).id == entry.id)
            break;
    if (i == entryList->size())
        throw string("XDBF: Error rewriting entry, entry not found.\n");

    // if the size has changed, then we need to reallocate memory
    if (entry.length != entryList->at(i).length)
    {
        DeallocateMemory(entryList->at(i).addressSpecifier, entryList->at(i).length);
        entryList->at(i).addressSpecifier = entry.length = GetSpecifier(AllocateMemory(entry.length));
        entryList->at(i).length = entry.length;
    }

    // write the entry
    io->setPosition(GetRealAddress(entry.addressSpecifier));
    io->write(entryBuffer, entry.length);

    // update the file
    UpdateEntry(&entryList->at(i));
    writeEntryListing();
}

void XDBF::DeleteEntry(XDBFEntry entry)
{
    // make sure that the entry exists
    vector<XDBFEntry> *entries;
    XDBFEntryGroup *group = 0;
    switch (entry.type)
    {
        case Achievement:
            entries = &achievements.entries;
            group = &achievements;
            break;
        case Image:
            entries = &images;
            break;
        case Setting:
            entries = &settings.entries;
            group = &settings;
            break;
        case Title:
            entries = &titlesPlayed.entries;
            group = &titlesPlayed;
            break;
        case String:
            entries = &strings;
            break;
        case AvatarAward:
            entries = &avatarAwards.entries;
            group = &avatarAwards;
            break;
        default:
            throw string("XDBF: Error deleting entry. Invalid entry type.\n");
    }
    DWORD index;
    for (index = 0; index < entries->size(); index++)
        if (entries->at(index).id == entry.id)
            break;
    // if the entry doesn't exist then we have some problems
    if (index == entries->size())
        throw string("XDBF: Error deleting entry. Specified entry doesn't exist.");

    // deallocate the entry's memory
    DeallocateMemory(GetRealAddress(entry.addressSpecifier), entry.length);

    // remove the entry from the listing
    entries->erase(entries->begin() + index);

    // delete the sync if needed
    switch (entry.type)
    {
        case Achievement:
        case Setting:
        case Title:
        case AvatarAward:
            // find the sync and delete it
            for (DWORD i = 0; i < group->syncs.synced.size(); i++)
                if (group->syncs.synced.at(i).entryID == entry.id)
                {
                    group->syncs.synced.erase(group->syncs.synced.begin() + i);
                    goto writeSyncs;
                }
            for (DWORD i = 0; i < group->syncs.toSync.size(); i++)
                if (group->syncs.toSync.at(i).entryID == entry.id)
                {
                    group->syncs.toSync.erase(group->syncs.toSync.begin() + i);
                    break;
                }

            writeSyncs:
                writeSyncList(&group->syncs);
                break;
        case Image:
        case String:
            break;
        default:
            throw string("XDBF: Error deleting entry. Invalid entry type.\n");
    }

    // re-write the entry table
    writeEntryListing();

    // update the header
    header.entryCount--;
    writeHeader();
}

void XDBF::ExtractEntry(XDBFEntry entry, BYTE *outBuffer)
{
    // seek to the begining of the entry
    io->setPosition(GetRealAddress(entry.addressSpecifier));

    // read the bytes of the entry
    io->readBytes(outBuffer, entry.length);
}

bool compareEntries(XDBFEntry a, XDBFEntry b)
{
    if (a.type < b.type)
        return true;
    else
        return a.id < b.id;
}
