#include "FatxIO.h"

FatxIO::FatxIO(DeviceIO *device, FatxFileEntry *entry) : device(device), entry(entry)
{
    pos = 0;
}

void FatxIO::SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg)
{
    // we can't seek past the file
    if (position > entry->fileSize && !(entry->fileAttributes & FatxDirectory))
        throw std::string("FATX: Cannot seek past the file size.\n");


    if (dir == std::ios_base::cur)
        position += pos;
    else if (dir == std::ios_base::end)
        position = (device->DriveLength() - position);

    pos = position;

    // calculate the actual offset on disk
    DWORD clusterIndex = position / entry->partition->clusterSize;

    if (clusterIndex >= entry->clusterChain.size())
        throw std::string("FATX: Cluster chain not sufficient enough for file size.\n");

    DWORD cluster = entry->clusterChain.at(clusterIndex);

    // calculate the read position
    DWORD startInCluster = (position % entry->partition->clusterSize);
    INT64 driveOff = (entry->partition->clusterStartingAddress + (entry->partition->clusterSize * (INT64)(cluster - 1))) + startInCluster;

    // this stores how many bytes we have until we reach another cluster
    maxReadConsecutive = entry->partition->clusterSize - startInCluster;

    // set the position
    device->SetPosition(driveOff);
}

void FatxIO::Flush()
{
    // nothing to flush since it's a physical drive
}

void FatxIO::Close()
{
    // nothing to close since this doesn't actually have a file open
}

UINT64 FatxIO::GetPosition()
{
    return pos;
}

FatxFileEntry FatxIO::GetFatxFileEntry()
{
    return *entry;
}

void FatxIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    // get the length
    DWORD origLen = len;

    while (len > 0)
    {
        // calculate how many bytes to read
        DWORD bytesToRead = (len <= maxReadConsecutive) ? len : maxReadConsecutive;
        device->ReadBytes(outBuffer + (origLen - len), bytesToRead);

        // update the position and length
        SetPosition(pos + bytesToRead);
        len -= bytesToRead;
    }
}

void FatxIO::WriteBytes(BYTE *buffer, DWORD len)
{
}

void FatxIO::SaveFile(std::string savePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL)
{
    // get the current position
    INT64 pos = device->GetPosition();

    // seek to the beggining of the file
    SetPosition(0);

    // open the new file
    FileIO outFile(savePath, true);

    // initialization
    BYTE *buffer = new BYTE[0x10000];
    DWORD fileLen = entry->fileSize;
    DWORD total = (fileLen + 0xFFFF) / 0x10000;
    DWORD cur = 0;

    // extract by 0x10000 byte buffers
    while (fileLen >= 0x10000)
    {
        ReadBytes(buffer, 0x10000);
        outFile.Write(buffer, 0x10000);
        fileLen -= 0x10000;

        // update progress
        if (progress)
            progress(arg, cur++, total);
    }

    // pick up any slack
    if (fileLen != 0)
    {
        ReadBytes(buffer, fileLen);
        outFile.Write(buffer, fileLen);
    }

    // update progress
    if (progress)
        progress(arg, total, total);

    // cleanup
    outFile.Close();
    delete[] buffer;

    // set the original position
    device->SetPosition(pos);
}
