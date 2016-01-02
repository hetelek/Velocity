#ifndef GDFX_H
#define GDFX_H

#include "winnames.h"
#include "IO/IndexableMultiFileIO.h"
#include <iostream>
#include <vector>

#define GDFX_HEADER_MAGIC       "MICROSOFT*XBOX*MEDIA"
#define GDFX_HEADER_MAGIC_LEN 	0x14

using std::string;
using std::vector;

struct GdfxHeader
{
    BYTE magic[0x14];   // MICROSOFT*XBOX*MEDIA
    DWORD rootSector;
    DWORD rootSize;
    WINFILETIME creationTime;
};

struct GdfxFileEntry
{
    // in the file
    DWORD unknown;
    DWORD sector;
    DWORD size;
    BYTE attributes;
    BYTE nameLen;
    string name;

    // extra stuff
    string filePath;
    vector<GdfxFileEntry> files;
    DWORD address;
    DWORD fileIndex;
};

enum GdfxDirentAttributesutes
{
    GdfxReadOnly = 0x01,
    GdfxHidden = 0x02,
    GdfxSystem = 0x04,
    GdfxDirectory = 0x10,
    GdfxArchive = 0x20,
    GdfxDevice = 0x40,
    GdfxNormal = 0x80
};

// TODO: make this work with the IO interface, and Write header

// read the GDFX header, seek io to position of header beforehand
void GdfxReadHeader(BaseIO *io, GdfxHeader *header);

// read the next file entry in the listing, reeturns false on listing end
bool GdfxReadFileEntry(BaseIO *io, GdfxFileEntry *entry);

// Write a file entry back to the listing
void GdfxWriteFileEntry(BaseIO *io, GdfxFileEntry *entry);

// order them so the directories come first
int DirectoryFirstCompareGdfxEntries(const GdfxFileEntry &a, const GdfxFileEntry &b);

#endif // GDFX_H
