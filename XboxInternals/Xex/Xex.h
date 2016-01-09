#ifndef XEX_H
#define XEX_H

#include "IO/BaseIO.h"
#include "IO/FileIO.h"
#include "XexDefinitions.h"

#include <string>

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT Xbox360Executable
{
public:
    Xbox360Executable(BaseIO *io);
    Xbox360Executable(std::string fileName);
    ~Xbox360Executable();

    std::vector<std::string> GetSystemImportLibraries();

    DWORD GetModuleFlags();

    ESRBRating GetEsrbRating();

    std::string GetEsrbRatingText();

    PEGIRating GetPegiRating();

    std::string GetPegiRatingText();

private:
    bool deleteIO;
    BaseIO *io;
    XexHeader header;
    std::vector<XexOptionalHeaderEntry> optionalHeaderData;
    std::vector<std::string> systemImportLibraries;

    // ratings
    ESRBRating esrbRating;
    PEGIRating pegiRating;
    PEGIFIRating pegifiRating;
    PEGIPTRating pegiptRating;
    PEGIBBFCRating pegibbfcRating;
    // there are still more

    void Parse();

    void ParseOptionalHeaderEntry(XexOptionalHeaderEntry *entry, int index);

    void ParseSystemImportLibraryTable(DWORD address);

    void ParseRatingInformation(DWORD address);
};

#endif // XEX_H
