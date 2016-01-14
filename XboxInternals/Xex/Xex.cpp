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

std::vector<XexResourceFileEntry> Xbox360Executable::GetResourceFileInfo() const
{
    return resourceFiles;
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

OFLCAURating Xbox360Executable::GetOflcAURating() const
{
    return oflcAURating;
}

std::string Xbox360Executable::GetOflcAURatingText() const
{
    switch (oflcAURating)
    {
        case OFLCAU_G:
            return "G";
        case OFLCAU_PG:
            return "PG";
        case OFLCAU_M:
            return "M";
        case OFLCAU_MA15_PLUS:
            return "MA15";
        default:
            return "Unrated";
    }
}

OFLCNZRating Xbox360Executable::GetOflcNZRating() const
{
    return oflcNZRating;
}

std::string Xbox360Executable::GetOflcNZRatingText() const
{
    switch (oflcNZRating)
    {
        case OFLCNZ_G:
            return "G";
        case OFLCNZ_PG:
            return "PG";
        case OFLCNZ_M:
            return "M";
        case OFLCNZ_MA15_PLUS:
            return "MA15";
        case OFLCNZ_R16:
            return "R16";
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

DWORD Xbox360Executable::GetAllowedMediaTypes() const
{
    return securityInfo.allowedMediaTypes;
}

void Xbox360Executable::ExtractDecryptedData(std::string path) const
{
    BYTE retailKey[XEX_AES_BLOCK_SIZE] = { 0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28, 0xFD, 0xC3,
            0x40, 0x58, 0x3F, 0xBB, 0x08, 0x96, 0xBF, 0x91 };

    BYTE sessionKey[XEX_AES_BLOCK_SIZE] = {0};
    BYTE bufferEnc[XEX_AES_BLOCK_SIZE];
    BYTE bufferDec[XEX_AES_BLOCK_SIZE];

    // decrypt the file key
    Botan::AES_128 aes;
    aes.set_key(retailKey, XEX_AES_BLOCK_SIZE);
    aes.decrypt(securityInfo.key, sessionKey);

    // set the key to the decrypted one
    aes.set_key(sessionKey, XEX_AES_BLOCK_SIZE);

    // seek to the beginning of the data
    io->SetPosition(header.dataAddress);

    // calculate how many blocks there are
    DWORD blockCount = (io->Length() - header.dataAddress) / XEX_AES_BLOCK_SIZE;

    // open an out file
    FileIO outFile(path, true);

    // wrong base file size

    // decrypt the data
    BYTE initializationVector[XEX_AES_BLOCK_SIZE] = {0};
    for (DWORD i = 0; i < blockCount; i++)
    {
        io->ReadBytes(bufferEnc, XEX_AES_BLOCK_SIZE);
        aes.decrypt(bufferEnc, bufferDec);

        for (DWORD x = 0; x < XEX_AES_BLOCK_SIZE; x++)
        {
            // xor the data with the old IV
            bufferDec[x] ^= initializationVector[x];

            // update the IV
            initializationVector[x] =  bufferEnc[x];
        }

        outFile.Write(bufferDec, XEX_AES_BLOCK_SIZE);
    }

    outFile.Close();
}

bool Xbox360Executable::HasRegion(XexRegion region)
{
    return !!(securityInfo.regions & region);
}

void Xbox360Executable::SetRegion(XexRegion region)
{
    securityInfo.regions |= region;
}

DWORD Xbox360Executable::GetImageFlags() const
{
    return securityInfo.imageFlags;
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
    header.headerAddress = io->ReadDword();
    header.optionalHeaderEntryCount = io->ReadDword();

    // read in all the optional header entries
    for (DWORD i = 0; i < header.optionalHeaderEntryCount; i++)
    {
        XexOptionalHeaderEntry entry;
        ParseOptionalHeaderEntry(&entry, i);
    }

    // read the security info
    io->SetPosition(header.headerAddress);
    securityInfo.size = io->ReadDword();
    securityInfo.imageSize = io->ReadDword();
    io->ReadBytes(securityInfo.pirsRsaSignature, 0x100);
    securityInfo.imageInfoSize = io->ReadDword();
    securityInfo.imageFlags = io->ReadDword();
    securityInfo.loadAddress = io->ReadDword();
    io->ReadBytes(securityInfo.sectionHash, 0x14);
    securityInfo.importTableSize = io->ReadDword();
    io->ReadBytes(securityInfo.importTableHash, 0x14);
    io->ReadBytes(securityInfo.mediaID, 0x10);
    io->ReadBytes(securityInfo.key, XEX_AES_BLOCK_SIZE);
    securityInfo.exportTableSize = io->ReadDword();
    io->ReadBytes(securityInfo.headerHash, 0x14);
    securityInfo.regions = io->ReadDword();
    securityInfo.allowedMediaTypes = io->ReadDword();

    // determine the page size
    if (securityInfo.imageFlags & PageSize4KB)
        pageSize = 0x1000;
    else
        pageSize = 0x10000;

    // read the sections
    DWORD sectionCount = io->ReadDword();
    for (DWORD i = 0; i < sectionCount; i++)
    {
        XexSectionEntry entry;

        // the bottom 4 bits are the type, the rest are the size of the entry in pages
        DWORD entryInfo = io->ReadDword();

        entry.type = (XexSectionType)(entryInfo & 0x0F);
        entry.totalSize = (entryInfo >> 4) * pageSize;

        io->ReadBytes(entry.hash, 0x14);
        sections.push_back(entry);
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
        case ResourceInfo:
            ParseResourceFileTable(headerEntry.data);
            break;
        case BaseFileDescriptor:
            ParseBaseFileDescriptor(headerEntry.data);
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

    /*Cero*/	io->ReadByte();
    /*USK*/		io->ReadByte();

    oflcAURating = io->ReadByte();
    oflcNZRating = io->ReadByte();

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

void Xbox360Executable::ParseResourceFileTable(DWORD address)
{
    io->SetPosition(address);
    DWORD tableSize = io->ReadDword();
    DWORD entryCount = (tableSize - 4) / XEX_RESOURCE_FILE_ENTRY_SIZE;

    for (DWORD i = 0; i < entryCount; i++)
    {
        // read the info from the file table
        XexResourceFileEntry entry;
        entry.name = io->ReadString(8);
        entry.address = io->ReadDword();
        entry.size = io->ReadDword();

        entry.dataOffset = entry.address - GetEntryPoint();
        resourceFiles.push_back(entry);
    }
}

void Xbox360Executable::ParseBaseFileDescriptor(DWORD address)
{
    io->SetPosition(address);

    // unknown
    io->ReadDword();

    encrypted = !!(io->ReadWord());

    if (io->ReadWord() != 2)
        compressionState = XexDecompressed;
    else if (io->ReadDword() <= 0x8000)
        compressionState = XexCompressed;
    else
        compressionState = XexSupercompressed;
}

void Xbox360Executable::ParseLANKey(DWORD address)
{
    io->SetPosition(address);
    io->ReadBytes(lanKey, XEX_LAN_KEY_SIZE);
}
