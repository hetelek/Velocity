#ifndef GDFX_H
#define GDFX_H

#include "winnames.h"
#include "IO/MultiFileIO.h"
#include <iostream>
#include <vector>

using std::string;
using std::vector;

struct GDFXHeader
{
    BYTE magic[0x14];   // MICROSOFT*XBOX*MEDIA
    DWORD rootSector;
    DWORD rootSize;
    FILETIME creationTime;
};

struct GDFXFileEntry
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
    vector<GDFXFileEntry> files;
};

enum DirentAttributes
{
    GdfxReadOnly = 0x01,
    GdfxHidden = 0x02,
    GdfxSystem = 0x04,
    GdfxDirectory = 0x10,
    GdfxArchive = 0x20,
    GdfxDevice = 0x40,
    GdfxNormal = 0x80
};

// TODO: make this work with the IO interface, and write header

// read the GDFX header, seek io to position of header beforehand
void GdfxReadHeader(MultiFileIO *io, GDFXHeader *header);

// read the next file entry in the listing, reeturns false on listing end
bool GdfxReadFileEntry(MultiFileIO *io, GDFXFileEntry *entry);

#endif // GDFX_H
