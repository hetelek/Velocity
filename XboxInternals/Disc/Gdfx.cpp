#include "Gdfx.h"

void GdfxReadHeader(BaseIO *io, GdfxHeader *header)
{
    // make sure that the byte order is little endian
    io->SetEndian(LittleEndian);

    // verify the magic
    io->ReadBytes((BYTE*)header->magic, GDFX_HEADER_MAGIC_LEN);
    if (memcmp(header->magic, GDFX_HEADER_MAGIC, GDFX_HEADER_MAGIC_LEN) != 0)
        throw string("GDFX: Invalid header magic\n");

    // read the rest of the header
    header->rootSector = io->ReadDword();
    header->rootSize = io->ReadDword();
    header->creationTime.dwLowDateTime = io->ReadDword();
    header->creationTime.dwHighDateTime = io->ReadDword();
}

bool GdfxReadFileEntry(BaseIO *io, GdfxFileEntry *entry)
{
    // everything's little endian, so let's be safe
    io->SetEndian(LittleEndian);

    // check to see if we're at the end
    DWORD nextBytes = io->ReadDword();
    if (nextBytes == 0xFFFFFFFF)
        return false;

    // parse the entry
    entry->unknown = nextBytes;
    entry->sector = io->ReadDword();
    entry->size = io->ReadDword();
    entry->attributes = io->ReadByte();
    entry->nameLen = io->ReadByte();
    entry->name = io->ReadString(entry->nameLen);

    return true;
}

void GdfxWriteFileEntry(BaseIO *io, GdfxFileEntry *entry)
{
    // everything's little endian, so let's be safe
    io->SetEndian(LittleEndian);

    // Write the entry
    io->Write(entry->unknown);
    io->Write(entry->sector);
    io->Write(entry->size);
    io->Write(entry->attributes);
    io->Write(entry->nameLen);
    io->Write(entry->name);
}

// this must be a < operator, if it returns true if they're equal then it'll break
// this will compare them so that the directories come first
int DirectoryFirstCompareGdfxEntries(const GdfxFileEntry &a, const GdfxFileEntry &b)
{
    int a_directory = a.attributes & GdfxDirectory;
    int b_directory = b.attributes & GdfxDirectory;

    return a_directory > b_directory;
}
