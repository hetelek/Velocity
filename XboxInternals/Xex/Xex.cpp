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

std::vector<std::string> Xbox360Executable::GetSystemImportLibraries()
{
    return systemImportLibraries;
}

DWORD Xbox360Executable::GetModuleFlags()
{
    return header.moduleFlags;
}

ESRBRating Xbox360Executable::GetEsrbRating()
{
    return esrbRating;
}

std::string Xbox360Executable::GetEsrbRatingText()
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
    headerEntry.id = io->ReadDword();
    headerEntry.data = io->ReadDword();

    // check to see if data outside the entry table needs to be read
    if (!headerEntry.IsInline())
    {
        switch (headerEntry.id)
        {
            case SystemImportLibraries:
                ParseSystemImportLibraryTable(headerEntry.data);
                break;
            case RatingInformation:
                ParseRatingInformation(headerEntry.data);
                break;
        }
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
