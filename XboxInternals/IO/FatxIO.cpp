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

    pos = position;

    // calculate the actual offset on disk
    DWORD clusterIndex = position / entry->partition->clusterSize;
    DWORD cluster = entry->clusterChain.at(clusterIndex);

    // calculate the read position
    DWORD startInCluster = (position % entry->partition->clusterSize);
    INT64 driveOff = (entry->partition->clusterStartingAddress + (entry->partition->clusterSize * (INT64)(cluster - 1))) + startInCluster;

    // this stores how many bytes we have until we reach another cluster
    maxReadConsecutive = entry->partition->clusterSize - startInCluster;

    // set the position
    device->SetPosition(driveOff);
}

UINT64 FatxIO::GetPosition()
{
}

void FatxIO::Close()
{
}

void FatxIO::Flush()
{
}

FatxFileEntry FatxIO::GetFatxFileEntry()
{
    return *entry;
}

void FatxIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    DWORD origLen = len;

    while (len > 0)
    {
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
