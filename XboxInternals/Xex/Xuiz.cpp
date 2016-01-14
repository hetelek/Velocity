#include "Xuiz.h"

Xuiz::Xuiz(std::string filePath) :
    freeIO(true)
{
    io = new FileIO(filePath);
    Parse();
}

Xuiz::Xuiz(BaseIO *io) :
    io(io), freeIO(false)
{
    Parse();
}

Xuiz::~Xuiz()
{
    if (freeIO)
        delete io;
}

std::vector<XuizFileEntry> Xuiz::GetFiles() const
{
    return files;
}

void Xuiz::ExtractFile(std::string outFile, XuizFileEntry *fileEntry) const
{
    BYTE *copyBuffer = new BYTE[XUIZ_COPY_BUFFER_SIZE];

    DWORD copyIterations = fileEntry->size / XUIZ_COPY_BUFFER_SIZE;
    if (fileEntry->size % XUIZ_COPY_BUFFER_SIZE != 0)
        copyIterations++;

    FileIO outIO(outFile, true);
    for (DWORD i = 0; i < copyIterations; i++)
    {
        // if it's the last iteration then be careful not read beyond the end of the file
        DWORD numBytesToCopy = XUIZ_COPY_BUFFER_SIZE;
        if (i + 1 == copyIterations && fileEntry->size % XUIZ_COPY_BUFFER_SIZE != 0)
            numBytesToCopy = fileEntry->size % XUIZ_COPY_BUFFER_SIZE;

        io->SetPosition(fileEntry->realAddress);
        io->ReadBytes(copyBuffer, numBytesToCopy);
        outIO.WriteBytes(copyBuffer, numBytesToCopy);
    }

    outIO.Close();
    delete copyBuffer;
}

void Xuiz::ExtractFile(std::string outFile, std::string xuizFilePath) const
{
    XuizFileEntry *entry = GetFileEntry(xuizFilePath);
    if (entry == NULL)
        throw std::string("Xuiz: Unable to find the file to extract.");

    ExtractFile(outFile, entry);
}

XuizFileEntry* Xuiz::GetFileEntry(std::string xuizFilePath) const
{
    for (size_t i = 0; i < files.size(); i++)
    {
        XuizFileEntry file = files.at(i);
        if (file.path == xuizFilePath)
            return &files.at(i);
    }

    return NULL;
}

void Xuiz::Parse()
{
    io->SetPosition(0);
    if (io->ReadDword() != XUIZ_MAGIC)
        throw std::string("XUIZ: Invalid magic.");

    // all unknown
    io->ReadDword();
    io->ReadDword();
    io->ReadDword();

    DWORD fileTableSize = io->ReadDword();
    WORD fileTableEntryCount = io->ReadWord();

    // read all the file entries
    for (WORD i = 0; i < fileTableEntryCount; i++)
    {
        XuizFileEntry entry;
        entry.size = io->ReadDword();
        entry.offset = io->ReadDword();
        entry.realAddress = XUIZ_HEADER_SIZE + fileTableSize + entry.offset;

        BYTE pathLength = io->ReadByte();
        entry.path = io->ReadString(pathLength);

        files.push_back(entry);
    }
}
