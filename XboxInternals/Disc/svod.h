#ifndef SVOD_H
#define SVOD_H

#include "IO/FileIO.h"
#include "IO/MultiFileIO.h"
#include "gdfx.h"
#include "Stfs/XContentHeader.h"
#include <iostream>
#include <vector>
#include "IO/SvodIO.h"
#include <algorithm>
#include "botan/botan.h"
#include "botan/sha160.h"

#include "XboxInternals_global.h"

using std::string;
using std::vector;

class XBOXINTERNALSSHARED_EXPORT SVOD
{
public:
    SVOD(string rootFile);
    ~SVOD();

    XContentHeader *metadata;
    vector<GDFXFileEntry> root;

    SvodIO GetSvodIO(GDFXFileEntry entry);

    SvodIO GetSvodIO(string path);

    // get the address and file index for a sector
    void SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex);

    // fix all of the hashes in the system
    void Rehash(void (*progress)(DWORD, DWORD, void*) = NULL, void *arg = NULL);

    // fix the RSA signature in the root descriptor
    void Resign(string kvPath);

    // write a file entry back to the system
    void WriteFileEntry(GDFXFileEntry *entry);

    // get the total number of sectors in the system
    DWORD GetSectorCount();

private:
    string contentDirectory;
    MultiFileIO *io;
    FileIO *rootFile;
    GDFXHeader header;
    DWORD baseAddress;
    DWORD offset;

    // parse the file listing
    void ReadFileListing(vector<GDFXFileEntry> *entryList, DWORD sector, int size, string path);

    // get a file entry from the path, must start with a /
    GDFXFileEntry GetFileEntry(string path, vector<GDFXFileEntry> *listing);

    // hash a 0x1000 byte block
    void HashBlock(BYTE *block, BYTE *outHash);
};

int compareFileEntries(GDFXFileEntry a, GDFXFileEntry b);

#endif // SVOD_H
