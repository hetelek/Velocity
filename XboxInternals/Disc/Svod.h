#ifndef SVOD_H
#define SVOD_H

#include "Utils.h"
#include "IO/FileIO.h"
#include "IO/IndexableMultiFileIO.h"
#include "IO/LocalIndexableMultiFileIO.h"
#include "IO/FatxIndexableMultiFileIO.h"
#include "IO/SvodIO.h"
#include "IO/FatxIO.h"
#include "Gdfx.h"
#include "Stfs/XContentHeader.h"
#include "Stfs/IXContentHeader.h"
#include "Fatx/FatxDrive.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include "botan/botan.h"
#include "botan/sha160.h"

#ifdef __WIN32
    #include "Shlwapi.h"
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include "XboxInternals_global.h"

using std::string;
using std::vector;

class XBOXINTERNALSSHARED_EXPORT SVOD : public IXContentHeader
{
public:
    static std::vector<std::string> GetDataFilePaths(std::string rootDescriptorPath);

    SVOD(string rootFile, FatxDrive *drive = NULL, bool readFileListing = true);
    ~SVOD();

    vector<GdfxFileEntry> root;

    // get a SvodIO for the given entry
    SvodIO GetSvodIO(GdfxFileEntry entry);

    // get a SvodIO for the given entry
    SvodIO GetSvodIO(string path);

    // get the address and file index for a sector
    void SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex);

    // fix all of the hashes in the system
    void Rehash(void (*progress)(DWORD, DWORD, void*) = NULL, void *arg = NULL);

    // fix the RSA signature in the root descriptor
    void Resign(string kvPath);

    // Write a file entry back to the system
    void WriteFileEntry(GdfxFileEntry *entry);

    // get the total number of sectors in the system
    DWORD GetSectorCount();

    std::string GetContentName();

    void GetFileListing();

private:
    string contentDirectory;
    IndexableMultiFileIO *io;
    BaseIO *rootFile;
    GdfxHeader header;
    DWORD baseAddress;
    DWORD offset;
    FatxDrive *drive;
    bool didReadFileListing;

    // parse the file listing
    void ReadFileListing(vector<GdfxFileEntry> *entryList, DWORD sector, int size, string path);

    // get a file entry from the path, must start with a /
    GdfxFileEntry GetFileEntry(string path, vector<GdfxFileEntry> *listing);

    // hash a 0x1000 byte block
    void HashBlock(BYTE *block, BYTE *outHash);
};

int compareFileEntries(const GdfxFileEntry &a, const GdfxFileEntry &b);

#endif // SVOD_H
