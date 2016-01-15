#ifndef ISO_H
#define ISO_H

// All credit for Xbox 360 ISO format goes to https://github.com/mborgerson/extract-xiso

#include "IO/BaseIO.h"
#include "IO/BigFileIO.h"
#include "Stfs/StfsPackage.h"
#include "Gdfx.h"
#include "Utils.h"

#include <string>
#include <algorithm>

#include "XboxInternals_global.h"

#define ISO_SECTOR_SIZE 	0x800
#define ISO_XGD1_ADDRESS 	0x10000
#define ISO_XGD2_ADDRESS 	0xFDA0000
#define ISO_XGD3_ADDRESS 	0x2090000

// this is 3.2768 MB
#define ISO_COPY_BUFFER_SIZE 	(ISO_SECTOR_SIZE * 1000)


class IsoIO;

class XBOXINTERNALSSHARED_EXPORT ISO
{
public:
    ISO(BaseIO *io);
    ISO(std::string filePath);
    ~ISO();

    std::vector<GdfxFileEntry> root;

    UINT64 SectorToAddress(DWORD sector);

    void GetFileListing();

    void ExtractFile(std::string outDirectory, const GdfxFileEntry *fileEntry, void (*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    void ExtractFile(std::string outDirectory, std::string filePath, void (*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    void ExtractAll(std::string outDirectory, void (*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    GdfxFileEntry *GetFileEntry(std::string filePath);

    IsoIO *GetIO(std::string filePath);

    IsoIO *GetIO(GdfxFileEntry *entry);

    std::string GetXGDVersion();

    UINT64 GetTotalSectors();

    // this will attempt to locate Stfs packages in the root and read their title name
    // older games don't have the packages there, they are usually avatar items and/or disc art
    std::wstring GetTitleName();

private:
    BaseIO *io;
    bool freeIO;
    UINT64 gdfxHeaderAddress;
    GdfxHeader gdfxHeader;
    bool didReadFileListing;
    std::string xgdVersion;
    std::wstring titleName;

    DWORD GetFileMagic(GdfxFileEntry *entry);

    void ParseISO();

    bool ValidGDFXHeader(UINT64 address);

    void ReadFileListing(std::vector<GdfxFileEntry> *entryList, DWORD sector, int size, string path);

    // recursively get the total number of copy iterations for all the files
    DWORD GetTotalCopyIterations(const vector<GdfxFileEntry> *entryList);

    void ExtractAllHelper(std::string outDirectory, std::vector<GdfxFileEntry> *entryList, void (*progress)(void *, DWORD, DWORD) = NULL, void *arg = NULL, DWORD *curProgress = NULL, DWORD totalProgress = 0);

    void ExtractFileHelper(std::string outDirectory, const GdfxFileEntry *toExtract, void (*progress)(void *, DWORD, DWORD) = NULL, void *arg = NULL, DWORD *curProgress = NULL, DWORD totalProgress = 0);
};

#endif // ISO_H
