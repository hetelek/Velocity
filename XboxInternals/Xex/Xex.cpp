#include "Xex.h"
#include "IO/XexZeroBasedCompressionIO.h"

Xbox360Executable::Xbox360Executable(BaseIO *io) :
    deleteIO(false), io(io), firstResourceFileAddr(0xFFFFFFFF), rawDataIO(NULL), imageBaseAddress(0),
    entryPoint(0), originalBaseAddress(0), defaultStackSize(0), defaultFileSystemCacheSize(0),
    defaultHeapSize(0), titleWorkspaceSize(0)
{
    Parse();
}

Xbox360Executable::Xbox360Executable(std::string fileName) :
    deleteIO(true), firstResourceFileAddr(0xFFFFFFFF), rawDataIO(NULL), imageBaseAddress(0),
    entryPoint(0), originalBaseAddress(0), defaultStackSize(0), defaultFileSystemCacheSize(0),
    defaultHeapSize(0), titleWorkspaceSize(0)
{
    io = new FileIO(fileName);

    Parse();
}

Xbox360Executable::~Xbox360Executable()
{
    if (deleteIO)
        delete io;

    if (rawDataIO)
        delete rawDataIO;

    if (!rawDataPath.empty())
        Utils::DeleteLocalFile(rawDataPath);
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

void Xbox360Executable::ExtractBaseImageFile(std::string outPath)
{
    BaseIO *rawDataIO = GetRawDataIO();
    DWORD baseImageSize = rawDataIO->Length();

    // if there are resources then the PE file runs right up until the first resource
    if (firstResourceFileAddr != 0xFFFFFFFF)
    {
        DWORD firstResourceFileDataOffset = firstResourceFileAddr - GetImageBaseAddress();
        baseImageSize = firstResourceFileDataOffset;
    }

    ExtractFromRawData(outPath, 0, baseImageSize);
}

void Xbox360Executable::ExtractResource(std::string resourceName, std::string outPath)
{
    // get the resource info
    XexResourceFileEntry entry;
    bool found = false;
    for (size_t i = 0; i < resourceFiles.size(); i++)
    {
        XexResourceFileEntry curEntry = resourceFiles.at(i);
        if (curEntry.name == resourceName)
        {
            found = true;
            entry = curEntry;
        }
    }

    if (!found)
        throw std::string("XEX: Couldn't find resource to extract.");

    ExtractFromRawData(outPath, entry.address - GetImageBaseAddress(), entry.size);
}

void Xbox360Executable::ExtractData(std::string path)
{
    if (compressionState == XexDecompressed && IsEncrypted())
        ExtractDecryptedData(path);
    else if (compressionState == XexCompressed)
        ExtractDecompressedData(path);
}

void Xbox360Executable::ExtractDecryptedData(std::string path) const
{
    BYTE bufferEnc[XEX_AES_BLOCK_SIZE];
    BYTE bufferDec[XEX_AES_BLOCK_SIZE];

    // set up the cipher
    Botan::AES_128 aes;
    aes.set_key(decryptedKey, XEX_AES_BLOCK_SIZE);

    // seek to the beginning of the data
    io->SetPosition(header.dataAddress);

    // calculate how many blocks there are
    DWORD blockCount = (io->Length() - header.dataAddress) / XEX_AES_BLOCK_SIZE;

    // open an out file
    FileIO outFile(path, true);

    // decrypt the data (AES-128 cbc)
    BYTE initializationVector[XEX_AES_BLOCK_SIZE] = {0};
    for (DWORD i = 0; i < blockCount; i++)
    {
        io->ReadBytes(bufferEnc, XEX_AES_BLOCK_SIZE);
        AesCbcDecrypt(&aes, initializationVector, bufferEnc, bufferDec);

        outFile.Write(bufferDec, XEX_AES_BLOCK_SIZE);
    }

    outFile.Close();
}

void Xbox360Executable::ExtractDecompressedData(std::string path)
{
    BYTE *copyBuffer = new BYTE[XEX_COPY_BUFFER_SIZE];
    FileIO outFile(path, true);

    // if it's encrypted we'll have to decrypt it
    Botan::AES_128 aes;
    BYTE iv[XEX_AES_BLOCK_SIZE] = {0};
    aes.set_key(decryptedKey, XEX_AES_BLOCK_SIZE);

    // seek to the beginning of the data
    io->SetPosition(header.dataAddress);

    // decompress the data
    for (DWORD i = 0; i < compressionBlocks.size(); i++)
    {
        XexCompressionBlock block = compressionBlocks.at(i);

        // determine how many copy iterations it will take
        DWORD copyIterations = block.size / XEX_COPY_BUFFER_SIZE;
        if (block.size % XEX_COPY_BUFFER_SIZE != 0)
            copyIterations++;

        // copy over the data
        for (DWORD x = 0; x < copyIterations; x++)
        {
            // determine the amount of bytes to copy
            DWORD bytesToCopy = XEX_COPY_BUFFER_SIZE;
            if (x + 1 == copyIterations && block.size % XEX_COPY_BUFFER_SIZE != 0)
                bytesToCopy = block.size % XEX_COPY_BUFFER_SIZE;

            // read in the data
            io->ReadBytes(copyBuffer, bytesToCopy);

            // decrypt it if necessary
            if (IsEncrypted())
            {
                // could be problematic
                DWORD aesBlocksInBuffer = bytesToCopy / XEX_AES_BLOCK_SIZE;
                for (DWORD y = 0; y < aesBlocksInBuffer; y++)
                {
                    BYTE *currentAesBlock = copyBuffer + y * XEX_AES_BLOCK_SIZE;
                    AesCbcDecrypt(&aes, iv, currentAesBlock, currentAesBlock);
                }
            }

            outFile.WriteBytes(copyBuffer, bytesToCopy);
        }

        // calculate the copy iterations for the null data
        memset(copyBuffer, 0, XEX_COPY_BUFFER_SIZE);
        copyIterations = block.nullSize / XEX_COPY_BUFFER_SIZE;
        if (block.nullSize % XEX_COPY_BUFFER_SIZE != 0)
            copyIterations++;

        // write all the null data
        for (DWORD x = 0; x < copyIterations; x++)
        {
            // determine the amount of bytes to copy
            DWORD bytesToCopy = XEX_COPY_BUFFER_SIZE;
            if (x + 1 == copyIterations && block.nullSize % XEX_COPY_BUFFER_SIZE != 0)
                bytesToCopy = block.nullSize % XEX_COPY_BUFFER_SIZE;

            outFile.WriteBytes(copyBuffer, bytesToCopy);
        }
    }

    outFile.Close();
    delete copyBuffer;
}

void Xbox360Executable::ExtractData2(std::string path)
{
    // create an io to read the data as it is in the file
    BaseIO *plaintextDataIO = io;
    plaintextDataIO->SetPosition(header.dataAddress);

    // check to see if it needs to be zero decompressed
    if (compressionBlocks.size() != 0)
        plaintextDataIO = new XexZeroBasedCompressionIO(plaintextDataIO, this);

    // calculate the total data size
    DWORD dataSize;
    if (compressionBlocks.size() == 0)
    {
        dataSize = io->Length() - header.dataAddress;
    }
    else
    {
        dataSize = 0;
        for (size_t i = 0; i < compressionBlocks.size(); i++)
        {
            dataSize += compressionBlocks.at(i).size;
            dataSize += compressionBlocks.at(i).nullSize;
        }
    }

    DWORD copyIterations = dataSize / XEX_COPY_BUFFER_SIZE;
    if (dataSize % XEX_COPY_BUFFER_SIZE != 0)
        copyIterations++;

    BYTE *copyBuffer = new BYTE[XEX_COPY_BUFFER_SIZE];

    // copy the data over in chunks
    FileIO outFile(path, true);
    for (DWORD i = 0; i < copyIterations; i++)
    {
        // determine the amount of bytes to copy
        DWORD bytesToCopy = XEX_COPY_BUFFER_SIZE;
        if (i + 1 == copyIterations && dataSize % XEX_COPY_BUFFER_SIZE != 0)
            bytesToCopy = dataSize % XEX_COPY_BUFFER_SIZE;

        // copy the bytes to the out file
        plaintextDataIO->ReadBytes(copyBuffer, bytesToCopy);
        outFile.Write(copyBuffer, bytesToCopy);
    }

    // cleanup
    if (plaintextDataIO != io)
        delete plaintextDataIO;
    delete copyBuffer;
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

DWORD Xbox360Executable::GetImageSize() const
{
    return securityInfo.imageSize;
}

BYTE *Xbox360Executable::GetMediaID()
{
    return securityInfo.mediaID;
}

BYTE *Xbox360Executable::GetKey()
{
    return securityInfo.key;
}

BYTE *Xbox360Executable::GetHeaderHash()
{
    return securityInfo.headerHash;
}

BYTE *Xbox360Executable::GetLANKey()
{
    return lanKey;
}

DWORD Xbox360Executable::GetTitleID() const
{
    return executionInfo.titleID;
}

XexEncryptionState Xbox360Executable::GetEncryptionState() const
{
    return encryptionState;
}

std::string Xbox360Executable::GetEncryptionStateStr() const
{
    switch (encryptionState)
    {
        case RetailEncrypted:
            return "Retail Encrypted";
        case DevKitEncrypted:
            return "DevKit Encrypted";
        case Decrypted:
            return "Decrypted";
        default:
            throw std::string("XEX: Unknown encryption state");
    }
}

bool Xbox360Executable::IsEncrypted() const
{
    return encrypted;
}

XexCompressionState Xbox360Executable::GetCompressionState() const
{
    return compressionState;
}

std::string Xbox360Executable::GetCompressionStateStr() const
{
    switch (compressionState)
    {
        case XexCompressed:
            return "Compressed";
        case XexSupercompressed:
            return "Super compressed";
        case XexDeltaCompressed:
            return "Delta compressed";
        case XexDecompressed:
            return "Decompressed";
        default:
            throw std::string("XEX: Invalid compression state.");
    }
}

void Xbox360Executable::AesCbcDecrypt(Botan::AES_128 *aes, BYTE *initializationVector, const BYTE *bufferEnc, BYTE *bufferDec) const
{
    BYTE encCopy[XEX_AES_BLOCK_SIZE];
    memcpy(encCopy, bufferEnc, XEX_AES_BLOCK_SIZE);

    aes->decrypt(bufferEnc, bufferDec);

    for (DWORD x = 0; x < XEX_AES_BLOCK_SIZE; x++)
    {
        // xor the data with the old IV
        bufferDec[x] ^= initializationVector[x];

        // update the IV
        initializationVector[x] = encCopy[x];
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

    // if it's encrypted figure out which key it uses
    const BYTE *key = XEX_RETAIL_KEY;
    if (IsEncrypted())
    {
        // preserve the position in the file, TryKey() changes it
        UINT64 position = io->GetPosition();

        // I don't think it says in the header which key it uses so we'll just try both
        if (TryKey(XEX_RETAIL_KEY))
        {
            encryptionState = RetailEncrypted;
            key = XEX_RETAIL_KEY;
        }
        else if (TryKey(XEX_DEVKIT_KEY))
        {
            encryptionState = DevKitEncrypted;
            key = XEX_DEVKIT_KEY;
        }
        else
            throw std::string("XEX: Unable to decrypt.");

        io->SetPosition(position);
    }
    else
    {
        encryptionState = Decrypted;
    }

    // decrypt the key in the file
    Botan::AES_128 aes;
    aes.set_key(key, XEX_AES_BLOCK_SIZE);
    aes.decrypt(securityInfo.key, decryptedKey);

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
        case ExecutionInfo:
            ParseExecutionInfo(headerEntry.data);
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

    /*DWORD totalLength = */ io->ReadDword();
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

    oflcAURating = (OFLCAURating)io->ReadByte();
    oflcNZRating = (OFLCNZRating)io->ReadByte();

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

        if (entry.address < firstResourceFileAddr)
            firstResourceFileAddr = entry.address;

        resourceFiles.push_back(entry);
    }
}

void Xbox360Executable::ParseBaseFileDescriptor(DWORD address)
{
    io->SetPosition(address);

    DWORD baseFileDescriptorSize = io->ReadDword();

    encrypted = !!(io->ReadWord());
    compressionState = (XexCompressionState)io->ReadWord();

    // read all the compression blocks
    DWORD blockCount = (baseFileDescriptorSize - 8) / XEX_COMPRESSION_BLOCK_SIZE;
    for (DWORD i = 0; i < blockCount; i++)
    {
        XexCompressionBlock block;
        block.size = io->ReadDword();
        block.nullSize = io->ReadDword();

        compressionBlocks.push_back(block);
    }
}

void Xbox360Executable::ParseLANKey(DWORD address)
{
    io->SetPosition(address);
    io->ReadBytes(lanKey, XEX_LAN_KEY_SIZE);
}

void Xbox360Executable::ParseExecutionInfo(DWORD address)
{
    io->SetPosition(address);

    executionInfo.mediaID = io->ReadDword();
    executionInfo.version = io->ReadDword();
    executionInfo.baseVersion = io->ReadDword();
    executionInfo.titleID = io->ReadDword();
    executionInfo.executionTable = io->ReadByte();
    executionInfo.platform = io->ReadByte();
    executionInfo.discNumber = io->ReadByte();
    executionInfo.discCount = io->ReadByte();
    executionInfo.savegameID = io->ReadDword();
}

void Xbox360Executable::ExtractFromRawData(std::string outPath, DWORD address, DWORD size)
{
    BaseIO *rawDataIO = GetRawDataIO();
    DWORD offset = rawDataIO->GetPosition();

    rawDataIO->SetPosition(offset + address);

    // extract the data
    BYTE *copyBuffer = new BYTE[XEX_COPY_BUFFER_SIZE];

    // determine how many copy iterations it will take
    DWORD copyIterations = size / XEX_COPY_BUFFER_SIZE;
    if (size % XEX_COPY_BUFFER_SIZE != 0)
        copyIterations++;

    FileIO outFile(outPath, true);
    for (DWORD i = 0; i < copyIterations; i++)
    {
        // determine the amount of bytes to copy
        DWORD bytesToCopy = XEX_COPY_BUFFER_SIZE;
        if (i + 1 == copyIterations && size % XEX_COPY_BUFFER_SIZE != 0)
            bytesToCopy = size % XEX_COPY_BUFFER_SIZE;

        rawDataIO->ReadBytes(copyBuffer, bytesToCopy);
        outFile.WriteBytes(copyBuffer, bytesToCopy);
    }

    outFile.Close();
    delete copyBuffer;
}

bool Xbox360Executable::TryKey(const BYTE *key)
{
    Botan::AES_128 aes;
    BYTE decryptedKeyBuffer[XEX_AES_BLOCK_SIZE];

    aes.set_key(key, XEX_AES_BLOCK_SIZE);
    aes.decrypt(securityInfo.key, decryptedKeyBuffer);

    aes.set_key(decryptedKeyBuffer, XEX_AES_BLOCK_SIZE);

    BYTE iv[XEX_AES_BLOCK_SIZE] = {0};
    BYTE encryptedBuffer[XEX_AES_BLOCK_SIZE] = {0};
    BYTE decryptedBuffer[XEX_AES_BLOCK_SIZE] = {0};

    // read in the first block
    io->SetPosition(header.dataAddress);
    io->ReadBytes(encryptedBuffer, XEX_AES_BLOCK_SIZE);

    AesCbcDecrypt(&aes, iv, encryptedBuffer, decryptedBuffer);

    // check to see if the windows PE magic is there
    MemoryIO peIO(decryptedBuffer, XEX_AES_BLOCK_SIZE);
    peIO.SetEndian(BigEndian);

    return peIO.ReadWord() == 0x4D5A; 	// 'MZ'
}

BaseIO *Xbox360Executable::GetRawDataIO()
{
    // check if we need to decompress/decrypt anything
    if (compressionState == XexDecompressed && !IsEncrypted())
    {
        io->SetPosition(header.dataAddress);
        return io;
    }
    else
    {
        // check to see if a rawDataIO already exists
        if (rawDataIO != NULL)
        {
            rawDataIO->SetPosition(0);
            return rawDataIO;
        }

        // create a temporary file with the cleaned data
        rawDataPath = Utils::GetTemporaryFileName();
        ExtractData(rawDataPath);

        rawDataIO = new FileIO(rawDataPath);
        return rawDataIO;
    }
}
