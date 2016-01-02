#ifndef ISO_H
#define ISO_H

// All credit for Xbox 360 ISO format goes to https://github.com/mborgerson/extract-xiso

#include "IO/BaseIO.h"
#include "IO/BigFileIO.h"
#include "Gdfx.h"

#include <string>

#include "XboxInternals_global.h"

#define ISO_SECTOR_SIZE 	0x800
#define ISO_XGD1_ADDRESS 	0x10000
#define ISO_XGD2_ADDRESS 	0xFDA0000
#define ISO_XGD3_ADDRESS 	0x2080000

class XBOXINTERNALSSHARED_EXPORT ISO
{
public:
    ISO(BaseIO *io);
    ISO(std::string filePath);
    ~ISO();

    std::vector<GdfxFileEntry> root;

    UINT64 SectorToAddress(DWORD sector);

    void GetFileListing();

private:
    BaseIO *io;
    bool freeIO;
    UINT64 gdfxHeaderAddress;
    GdfxHeader gdfxHeader;
    bool didReadFileListing;

    void ParseISO();

    bool ValidGDFXHeader(UINT64 address);

    void ReadFileListing(vector<GdfxFileEntry> *entryList, DWORD sector, int size, string path);
};

#endif // ISO_H
