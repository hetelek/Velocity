#include "SvodIO.h"

SvodIO::SvodIO(XContentHeader *metadata, GDFXFileEntry entry, MultiFileIO *io) :
    BaseIO(), io(io), metadata(metadata), fileEntry(entry), pos(0)
{
    // seek to the file's begining
    DWORD addr, index;
    SectorToAddress(entry.sector, &addr, &index);
    io->SetPosition(addr, index);
}

void SvodIO::SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex)
{
    DWORD trueSector = (sector - (metadata->svodVolumeDescriptor.dataBlockOffset * 2)) % 0x14388;
    *addressInDataFile = trueSector * 0x800;
    *dataFileIndex = (sector - (metadata->svodVolumeDescriptor.dataBlockOffset * 2)) / 0x14388;

    // for the master hash table
    *addressInDataFile += 0x1000;
    // for the GdfxHeader
    *addressInDataFile += 0x1000;
    // for the data hash table(s)
    *addressInDataFile += ((trueSector / 0x198) + ((trueSector % 0x198 == 0 && trueSector != 0) ? 0 : 1)) * 0x1000;
}

void SvodIO::SetPosition(DWORD address)
{
    DWORD baseAddress, baseIndex;
    SectorToAddress(fileEntry.sector, &baseAddress, &baseIndex);
}

void SvodIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    // get the current position
    DWORD addr, index;
    io->GetPosition(&addr, &index);

    // calculate the amount of bytes until the next hash table
    DWORD bytesUntilTable = 0xCC000 - ((addr - 0x2000) % 0xCD000);

    // calculate the amount of bytes to read
    DWORD bytesToRead = (bytesUntilTable > len) ? len : bytesUntilTable;

    // read the bytes before the table
    io->ReadBytes(outBuffer, bytesToRead);
    outBuffer += bytesToRead;
    len -= bytesToRead;

    // read in between all the hash tables
    while (len >= 0xCC000)
    {
        // check to see if we're at the end of a data file
        io->GetPosition(&addr, &index);
        if (addr == 0xA290000)
            io->SetPosition(0, index + 1);
        else
            io->SetPosition(addr + 0x1000);

        io->ReadBytes(outBuffer, 0xCC000);
        outBuffer += 0xCC000;
        len -= 0xCC000;
    }

    if (len != 0)
    {
        // check to see if we're at the end of a data file
        io->GetPosition(&addr, &index);
        if (addr == 0xA290000)
            io->SetPosition(0, index + 1);
        else
            io->SetPosition(addr + 0x1000);

        io->ReadBytes(outBuffer, len);
    }

    pos += len;
}

void SvodIO::WriteBytes(BYTE *buffer, DWORD len)
{
    // get the current position
    DWORD addr, index;
    io->GetPosition(&addr, &index);

    // calculate the amount of bytes until the next hash table
    DWORD bytesUntilTable = 0xCC000 - ((addr - 0x2000) % 0xCD000);

    // calculate the amount of bytes to read
    DWORD bytesToWrite = (bytesUntilTable > len) ? len : bytesUntilTable;

    // write the bytes before the table
    io->WriteBytes(buffer, bytesToWrite);
    buffer += bytesToWrite;
    len -= bytesToWrite;

    // write in between all the hash tables
    while (len >= 0xCC000)
    {
        // check to see if we're at the end of a data file
        io->GetPosition(&addr, &index);
        if (addr == 0xA290000)
            io->SetPosition(0, index + 1);
        else
            io->SetPosition(addr + 0x1000);

        io->WriteBytes(buffer, 0xCC000);
        buffer += 0xCC000;
        len -= 0xCC000;
    }

    if (len != 0)
    {
        // check to see if we're at the end of a data file
        io->GetPosition(&addr, &index);
        if (addr == 0xA290000)
            io->SetPosition(0, index + 1);
        else
            io->SetPosition(addr + 0x1000);

        io->WriteBytes(buffer, len);
    }

    pos += len;
}

void SvodIO::SaveFile(string savePath, void(*progress)(void*, DWORD, DWORD), void *arg)
{
    FileIO outFile(savePath, true);
    BYTE *buffer = new BYTE[0x10000];
    DWORD fileLen = fileEntry.size;
    DWORD total = (fileLen + 0xFFFF) / 0x10000;
    DWORD cur = 0;

    while (fileLen >= 0x10000)
    {
        ReadBytes(buffer, 0x10000);
        outFile.write(buffer, 0x10000);
        fileLen -= 0x10000;

        if (progress)
            progress(arg, cur++, total);
    }

    if (fileLen != 0)
    {
        ReadBytes(buffer, fileLen);
        outFile.write(buffer, fileLen);
    }

    if (progress)
        progress(arg, total, total);

    outFile.close();
    delete[] buffer;
}

void SvodIO::OverwriteFile(string inPath, void (*progress)(void *, DWORD, DWORD), void *arg)
{
    FileIO inFile(inPath);

    // make sure that the files are the same size
    inFile.setPosition(0, ios_base::end);
    if (inFile.getPosition() != fileEntry.size)
        throw string("SVOD: Cannot overwrite file of different length.\n");

    BYTE *buffer = new BYTE[0x10000];
    DWORD fileLen = fileEntry.size;
    DWORD total = (fileLen + 0xFFFF) / 0x10000;
    DWORD cur = 0;

    inFile.setPosition(0);

    while (fileLen >= 0x10000)
    {
        inFile.readBytes(buffer, 0x10000);
        WriteBytes(buffer, 0x10000);
        fileLen -= 0x10000;

        if (progress)
            progress(arg, cur++, total);
    }

    if (fileLen != 0)
    {
        inFile.readBytes(buffer, fileLen);
        WriteBytes(buffer, fileLen);
    }

    if (progress)
        progress(arg, total, total);

    inFile.close();
    delete[] buffer;
}

void SvodIO::Close()
{
    //io->Close();
}
