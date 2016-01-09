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

    std::vector<std::string> GetSystemImportLibraries() const;

    std::vector<XexStaticLibraryInfo> GetStaticLibraries() const;

    DWORD GetModuleFlags() const;

    ESRBRating GetEsrbRating() const;

    std::string GetEsrbRatingText() const;

    PEGIRating GetPegiRating() const;

    std::string GetPegiRatingText() const;

    std::string GetOriginalPEImageName() const;

    DWORD GetImageBaseAddress() const;

    DWORD GetEntryPoint() const;

    DWORD GetOriginalBaseAddress() const;

    DWORD GetDefaultStackSize() const;

    DWORD GetDefaultFileSystemCacheSize() const;

    DWORD GetDefaultHeapSize() const;

    DWORD GetTitleWorkspaceSize() const;

private:
    bool deleteIO;
    BaseIO *io;
    XexHeader header;
    std::vector<XexOptionalHeaderEntry> optionalHeaderData;
    std::vector<std::string> systemImportLibraries;
    std::vector<XexStaticLibraryInfo> staticLibraries;
    std::string originalPEImageName;

    // ratings
    ESRBRating esrbRating;
    PEGIRating pegiRating;
    PEGIFIRating pegifiRating;
    PEGIPTRating pegiptRating;
    PEGIBBFCRating pegibbfcRating;
    // there are still more

    DWORD imageBaseAddress;
    DWORD entryPoint;
    DWORD originalBaseAddress;
    DWORD defaultStackSize;
    DWORD defaultFileSystemCacheSize;
    DWORD defaultHeapSize;
    DWORD titleWorkspaceSize;


    void Parse();

    void ParseOptionalHeaderEntry(XexOptionalHeaderEntry *entry, int index);

    void ParseSystemImportLibraryTable(DWORD address);

    void ParseRatingInformation(DWORD address);

    void ParseStaticLibraryTable(DWORD address);

    void ParseOriginalPEImageName(DWORD address);
};

#endif // XEX_H
