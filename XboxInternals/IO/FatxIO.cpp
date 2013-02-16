#include "FatxIO.h"

FatxIO::FatxIO(DeviceIO *device, FatxFileEntry *entry) : device(device), entry(entry)
{
    pos = 0;
}

void FatxIO::SetPosition(DWORD position)
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
    INT64 pos = device->Position();

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
        outFile.write(buffer, 0x10000);
        fileLen -= 0x10000;

        // update progress
        if (progress)
            progress(arg, cur++, total);
    }

    // pick up any slack
    if (fileLen != 0)
    {
        ReadBytes(buffer, fileLen);
        outFile.write(buffer, fileLen);
    }

    // update progress
    if (progress)
        progress(arg, total, total);

    // cleanup
    outFile.close();

    // set the original position
    device->SetPosition(pos);
}
