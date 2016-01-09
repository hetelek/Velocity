#include "Xex.h"

Xbox360Executable::Xbox360Executable(BaseIO *io) :
    io(io), deleteIO(false)
{
    Parse();
}

Xbox360Executable::Xbox360Executable(std::string fileName) :
    deleteIO(true)
{
    io = new FileIO(fileName);

    Parse();
}

Xbox360Executable::~Xbox360Executable()
{
    if (deleteIO)
        delete io;
}

std::vector<std::string> Xbox360Executable::GetSystemImportLibraries() const
{
    return systemImportLibraries;
}

std::vector<XexStaticLibraryInfo> Xbox360Executable::GetStaticLibraries() const
{
    return staticLibraries;
}

DWORD Xbox360Executable::GetModuleFlags() const
{
    return header.moduleFlags;
}

ESRBRating Xbox360Executable::GetEsrbRating() const
{
    return esrbRating;
}

std::string Xbox360Executable::GetEsrbRatingText() const
{
    switch (esrbRating)
    {
        case ESRB_EC:
            return "EC";
        case ESRB_E:
            return "E";
        case ESRB_10:
            return "E10";
        case ESRB_T:
            return "T";
        case ESRB_M:
            return "M";
        default:
            return "RP";
    }
}

PEGIRating Xbox360Executable::GetPegiRating() const
{
    return pegiRating;
}

std::string Xbox360Executable::GetPegiRatingText() const
{
    switch (pegiRating)
    {
        case PEGI_3:
            return "3";
        case PEGI_7:
            return "7";
        case PEGI_12:
            return "12";
        case PEGI_16:
            return "16";
        case PEGI_18:
            return "18";
        default:
            return "Unrated";
    }
}

std::string Xbox360Executable::GetOriginalPEImageName() const
{
    return originalPEImageName;
}

DWORD Xbox360Executable::GetImageBaseAddress() const
{
    return imageBaseAddress;
}

DWORD Xbox360Executable::GetEntryPoint() const
{
    return entryPoint;
}

DWORD Xbox360Executable::GetOriginalBaseAddress() const
{
    return originalBaseAddress;
}

DWORD Xbox360Executable::GetDefaultStackSize() const
{
    return defaultStackSize;
}

DWORD Xbox360Executable::GetDefaultFileSystemCacheSize() const
{
    return defaultFileSystemCacheSize;
}

DWORD Xbox360Executable::GetDefaultHeapSize() const
{
    return defaultHeapSize;
}

DWORD Xbox360Executable::GetTitleWorkspaceSize() const
{
    return titleWorkspaceSize;
}

void Xbox360Executable::Parse()
{
    io->SetPosition(0);
    io->SetEndian(BigEndian);

    // read the header
    header.magic = io->ReadDword();
    if (header.magic != XEX_HEADER_MAGIC)
        throw std::string("XEX: Invalid header magic.");
    header.moduleFlags = io->ReadDword();
    header.dataAddress = io->ReadDword();
    header.reserved = io->ReadDword();
    header.fileHeaderAddress = io->ReadDword();
    header.optionalHeaderEntryCount = io->ReadDword();

    // read in all the optional header entries
    for (DWORD i = 0; i < header.optionalHeaderEntryCount; i++)
    {
        XexOptionalHeaderEntry entry;
        ParseOptionalHeaderEntry(&entry, i);
    }
}

void Xbox360Executable::ParseOptionalHeaderEntry(XexOptionalHeaderEntry *entry, int index)
{
    // seek to the beginning of the entry
    io->SetPosition(XEX_HEADER_SIZE + index * XEX_OPTIONAL_HEADER_ENTRY_SIZE);

    XexOptionalHeaderEntry headerEntry;
    headerEntry.id = (XexOptionHeaderEntryID)io->ReadDword();
    headerEntry.data = io->ReadDword();

    switch (headerEntry.id)
    {
        case SystemImportLibraries:
            ParseSystemImportLibraryTable(headerEntry.data);
            break;
        case RatingInformation:
            ParseRatingInformation(headerEntry.data);
            break;
        case StaticLibraries:
            ParseStaticLibraryTable(headerEntry.data);
            break;
        case OriginalPEImageName:
            ParseOriginalPEImageName(headerEntry.data);
            break;
        case ImageBaseAddress:
            imageBaseAddress = headerEntry.data;
            break;
        case EntryPoint:
            entryPoint = headerEntry.data;
            break;
        case OriginalBaseAddress:
            originalBaseAddress = headerEntry.data;
            break;
        case DefaultStackSize:
            defaultStackSize = headerEntry.data;
            break;
        case DefaultFileSystemCacheSize:
            defaultFileSystemCacheSize = headerEntry.data;
            break;
        case DefaultHeapSize:
            defaultHeapSize = headerEntry.data;
            break;
        case TitleWorkspaceSize:
            titleWorkspaceSize = headerEntry.data;
            break;
    }
}

void Xbox360Executable::ParseSystemImportLibraryTable(DWORD address)
{
    io->SetPosition(address);

    // unknown
    io->ReadDword();

    DWORD totalLength = io->ReadDword();
    DWORD importLibraryCount = io->ReadDword();

    // read the import library names
    for (DWORD i = 0; i < importLibraryCount; i++)
        systemImportLibraries.push_back(io->ReadString());
}

void Xbox360Executable::ParseRatingInformation(DWORD address)
{
    io->SetPosition(address);

    esrbRating = (ESRBRating)io->ReadByte();
    pegiRating = (PEGIRating)io->ReadByte();
    pegifiRating = (PEGIFIRating)io->ReadByte();
    pegiptRating = (PEGIPTRating)io->ReadByte();
    pegibbfcRating = (PEGIBBFCRating)io->ReadByte();

    // there are more
}

void Xbox360Executable::ParseStaticLibraryTable(DWORD address)
{
    io->SetPosition(address);

    DWORD staticLibraryStructSize = io->ReadDword();
    DWORD libraryCount = (staticLibraryStructSize - 4) / XEX_STATIC_LIBRARY_ENTRY_SIZE;

    for (DWORD i = 0; i < libraryCount; i++)
    {
        XexStaticLibraryInfo staticLibInfo;
        staticLibInfo.name = io->ReadString(8);

        staticLibInfo.version.major = io->ReadWord();
        staticLibInfo.version.minor = io->ReadWord();
        staticLibInfo.version.build = io->ReadWord();
        staticLibInfo.version.revision = io->ReadWord();

        staticLibraries.push_back(staticLibInfo);
    }
}

void Xbox360Executable::ParseOriginalPEImageName(DWORD address)
{
    // seek past the length at the beginning
    io->SetPosition(address + 4);

    originalPEImageName = io->ReadString();
}
