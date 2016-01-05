#include "ISO.h"

ISO::ISO(BaseIO *io) :
    freeIO(false), didReadFileListing(false)
{
    ParseISO();
}

ISO::ISO(std::string filePath) :
    io(new BigFileIO(filePath)), freeIO(true), didReadFileListing(false)
{
    ParseISO();
}

ISO::~ISO()
{
    if (freeIO)
        delete io;
}

UINT64 ISO::SectorToAddress(DWORD sector)
{
    return (UINT64)sector * ISO_SECTOR_SIZE + gdfxHeaderAddress - ISO_XGD1_ADDRESS;
}

void ISO::GetFileListing()
{
    if (!didReadFileListing)
    {
        didReadFileListing = true;
        ReadFileListing(&root, gdfxHeader.rootSector, gdfxHeader.rootSize, "");
    }
}

void ISO::ExtractFile(GdfxFileEntry fileEntry, void (*progress)(void*, DWORD, DWORD) = NULL, void *arg)
{
    DWORD bufferSize = ISO_SECTOR_SIZE * 100;
    BYTE *copyBuffer = new BYTE[bufferSize];



    delete copyBuffer;
}

void ISO::ExtractAll(std::string outDirectory, void (*progress)(void*, DWORD, DWORD), void *arg)
{
    // calculate the total number of copy iterations for reporting progress
    UINT64 totalSectors = GetTotalSectors(&root);
    DWORD totalCopyIterations = (totalSectors * ISO_SECTOR_SIZE) / ISO_COPY_BUFFER_SIZE;

    if (totalCopyIterations % ISO_COPY_BUFFER_SIZE != 0)
        totalCopyIterations++;

    DWORD curProgress = 0;
    ExtractAllHelper(outDirectory, &root, progress, arg, &curProgress, &totalCopyIterations);
}

void ISO::ParseISO()
{
    // first we need to find the address of the GDFX header, that depends on the XGD (Xbox game disc) version
    if (ValidGDFXHeader(ISO_XGD1_ADDRESS))
        gdfxHeaderAddress = ISO_XGD1_ADDRESS;
    else if (ValidGDFXHeader(ISO_XGD2_ADDRESS))
        gdfxHeaderAddress = ISO_XGD2_ADDRESS;
    else if (ValidGDFXHeader(ISO_XGD3_ADDRESS))
        gdfxHeaderAddress = ISO_XGD3_ADDRESS;
    else
        throw std::string("ISO: Invalid Xbox 360 ISO.");

    // parse the GDFX header
    io->SetPosition(gdfxHeaderAddress);
    GdfxReadHeader(io, &gdfxHeader);
}

bool ISO::ValidGDFXHeader(UINT64 address)
{
    BYTE gdfx_magic_buffer[GDFX_HEADER_MAGIC_LEN];

    io->SetPosition(address);
    io->ReadBytes(gdfx_magic_buffer, GDFX_HEADER_MAGIC_LEN);

    return memcmp(gdfx_magic_buffer, GDFX_HEADER_MAGIC, GDFX_HEADER_MAGIC_LEN) == 0;
}

void ISO::ReadFileListing(vector<GdfxFileEntry> *entryList, DWORD sector, int size, string path)
{
    // seek to the start of the directory listing
    UINT64 entryAddress = SectorToAddress(sector);
    io->SetPosition(entryAddress);

    GdfxFileEntry current;
    while (true)
    {
        current.address = io->GetPosition();
        current.fileIndex = 0;

        // make sure we're not at the end of the file listing
        if (!GdfxReadFileEntry(io, &current) && size != 0)
            break;

        // if it's a directory, then seek to it and read it's contents
        if (current.attributes & GdfxDirectory)
        {
            // preserve the current positon
            UINT64 seekAddr = io->GetPosition();

            ReadFileListing(&current.files, current.sector, current.size, path + current.name + "/");

            // reset position to current listing
            io->SetPosition(seekAddr);
        }

        current.filePath = path;
        entryList->push_back(current);

        // seek to the next entry
        entryAddress += (current.nameLen + 0x11) & 0xFFFFFFFC;
        io->SetPosition(entryAddress);

        // check for end
        DWORD nextBytes = io->ReadDword();
        if (nextBytes == 0xFFFFFFFF)
        {
            if ((size - ISO_SECTOR_SIZE) <= 0)
            {
                // sort the file entries so that directories are first
                std::sort(entryList->begin(), entryList->end(), DirectoryFirstCompareGdfxEntries);
                return;
            }
            else
            {
                size -= ISO_SECTOR_SIZE;
                entryAddress = SectorToAddress(++sector);
                io->SetPosition(entryAddress);
            }
        }

        // back up to the entry
        io->SetPosition(entryAddress);

        // reset the directory
        current.files.clear();
    }

    std::sort(entryList->begin(), entryList->end(), DirectoryFirstCompareGdfxEntries);
}

UINT64 ISO::GetTotalSectors(const vector<GdfxFileEntry> *entryList)
{
    UINT64 totalSectors = 0;
    for (size_t i = 0; i < entryList->size(); i++)
    {
        GdfxFileEntry entry = entryList->at(i);
        if (entry.attributes & GdfxDirectory)
            totalSectors += GetTotalSectors(&entry.files);
        totalSectors += entry.size / ISO_SECTOR_SIZE;
    }

    return totalSectors;
}

void ISO::ExtractAllHelper(std::string outDirectory, std::vector<GdfxFileEntry> *entryList, void (*progress)(void*, DWORD, DWORD), void *arg, DWORD *curProgress, const DWORD *totalProgress)
{
    BYTE *copyBuffer = new BYTE[ISO_COPY_BUFFER_SIZE];

    // extract all the files in the entry list
    for (size_t i = 0; i < entryList->size(); i++)
    {
        GdfxFileEntry entry = entryList->at(i);

        // if it's a directory then extract all the files inside it
        if (entry.attributes & GdfxDirectory)
        {
            Utils::CreateLocalDirectory(outDirectory + "/" + entry.filePath + entry.name);
            ExtractAllHelper(outDirectory, &entry.files, progress, arg, curProgress, totalProgress);
        }
        else
        {
            // create a new file on the local disk
            Utils::CreateLocalDirectory(outDirectory + "/" + entry.filePath);
            std::string outFilePath = outDirectory + "/" + entry.filePath + entry.name;
            BigFileIO extractedFile(outFilePath, true);

            DWORD totalReads = entry.size / ISO_COPY_BUFFER_SIZE;
            if (entry.size % ISO_COPY_BUFFER_SIZE != 0)
                totalReads++;

            // seek to the beginning of the file
            UINT64 readAddress = SectorToAddress(entry.sector);
            io->SetPosition(readAddress);

            // keep reading into the copy buffer and writing to the local disk until the entire file has been extracted
            for (DWORD x = 0; x < totalReads; x++)
            {
                // if we're at the end of the file then we won't need to read ISO_COPY_BUFFER_SIZE bytes
                DWORD numBytesToCopy = ISO_COPY_BUFFER_SIZE;
                if (x == totalReads - 1 && entry.size % ISO_COPY_BUFFER_SIZE != 0)
                     numBytesToCopy = entry.size % ISO_COPY_BUFFER_SIZE;

                // copy over numBytesToCopy bytes
                io->ReadBytes(copyBuffer, numBytesToCopy);
                extractedFile.WriteBytes(copyBuffer, numBytesToCopy);

                (*curProgress)++;

                if (progress)
                    progress(arg, *curProgress, *totalProgress);
            }

            extractedFile.Close();
        }
    }

    delete copyBuffer;
}
