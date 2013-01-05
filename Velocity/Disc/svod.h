#ifndef SVOD_H
#define SVOD_H

#include "FileIO.h"
#include "IO/MultiFileIO.h"
#include "gdfx.h"
#include "Stfs/XContentHeader.h"
#include <iostream>
#include <vector>
#include "IO/SvodIO.h"
#include <algorithm>
#include "botan/botan.h"
#include "botan/sha160.h"

using std::string;
using std::vector;

class SVOD
{
public:
    SVOD(string rootFile);

    XContentHeader *metadata;
    vector<GDFXFileEntry> root;

    SvodIO GetSvodIO(GDFXFileEntry entry);

    SvodIO GetSvodIO(string path);

    // get the address and file index for a sector
    void SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex);

    // fix all of the hashes in the system
    void Rehash();

private:
    string contentDirectory;
    MultiFileIO *io;
    GDFXHeader header;

    // parse the file listing
    void ReadFileListing(vector<GDFXFileEntry> *entryList, DWORD sector, DWORD size, string path);

    // get a file entry from the path, must start with a /
    GDFXFileEntry GetFileEntry(string path, vector<GDFXFileEntry> *listing);

    // hash a 0x1000 byte block
    void HashBlock(BYTE *block, BYTE *outHash);
};

int compareFileEntries(GDFXFileEntry a, GDFXFileEntry b);

#endif // SVOD_H
