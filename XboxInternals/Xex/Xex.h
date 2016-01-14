#ifndef XEX_H
#define XEX_H

#include "IO/BaseIO.h"
#include "IO/FileIO.h"
#include "XexDefinitions.h"
#include "Utils.h"

#include <string>

#include <botan/botan.h>
#include <botan/sha160.h>
#include <botan/aes.h>

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT Xbox360Executable
{
public:
    Xbox360Executable(BaseIO *io);
    Xbox360Executable(std::string fileName);
    ~Xbox360Executable();

    std::vector<std::string> GetSystemImportLibraries() const;

    std::vector<XexStaticLibraryInfo> GetStaticLibraries() const;

    std::vector<XexResourceFileEntry> GetResourceFileInfo() const;

    DWORD GetModuleFlags() const;

    ESRBRating GetEsrbRating() const;

    std::string GetEsrbRatingText() const;

    PEGIRating GetPegiRating() const;

    std::string GetPegiRatingText() const;

    OFLCAURating GetOflcAURating() const;

    std::string GetOflcAURatingText() const;

    OFLCNZRating GetOflcNZRating() const;

    std::string GetOflcNZRatingText() const;

    std::string GetOriginalPEImageName() const;

    DWORD GetImageBaseAddress() const;

    DWORD GetEntryPoint() const;

    DWORD GetOriginalBaseAddress() const;

    DWORD GetDefaultStackSize() const;

    DWORD GetDefaultFileSystemCacheSize() const;

    DWORD GetDefaultHeapSize() const;

    DWORD GetTitleWorkspaceSize() const;

    DWORD GetAllowedMediaTypes() const;

    void ExtractDecryptedData(std::string path) const;

    bool HasRegion(XexRegion region);

    void SetRegion(XexRegion region);

private:
    bool deleteIO;
    BaseIO *io;
    XexHeader header;
    DWORD pageSize;

    std::vector<XexOptionalHeaderEntry> optionalHeaderData;
    std::vector<XexStaticLibraryInfo> staticLibraries;
    std::vector<XexResourceFileEntry> resourceFiles;
    std::vector<XexSectionEntry> sections;
    std::vector<std::string> systemImportLibraries;
    std::string originalPEImageName;

    // ratings
    ESRBRating esrbRating;
    PEGIRating pegiRating;
    PEGIFIRating pegifiRating;
    PEGIPTRating pegiptRating;
    PEGIBBFCRating pegibbfcRating;
    OFLCAURating oflcAURating;
    OFLCNZRating oflcNZRating;
    // there are still more

    DWORD imageBaseAddress;
    DWORD entryPoint;
    DWORD originalBaseAddress;
    DWORD defaultStackSize;
    DWORD defaultFileSystemCacheSize;
    DWORD defaultHeapSize;
    DWORD titleWorkspaceSize;

    XexSecurityInfo securityInfo;
    bool encrypted;
    XexCompressionState compressionState;
    BYTE lanKey[XEX_LAN_KEY_SIZE];


    void Parse();

    void ParseOptionalHeaderEntry(XexOptionalHeaderEntry *entry, int index);

    void ParseSystemImportLibraryTable(DWORD address);

    void ParseRatingInformation(DWORD address);

    void ParseStaticLibraryTable(DWORD address);

    void ParseOriginalPEImageName(DWORD address);

    void ParseResourceFileTable(DWORD address);

    void ParseBaseFileDescriptor(DWORD address);

    void ParseLANKey(DWORD address);
};

#endif // XEX_H
