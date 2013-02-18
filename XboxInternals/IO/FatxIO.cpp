#include "FatxIO.h"

FatxIO::FatxIO(DeviceIO *device, FatxFileEntry *entry) : device(device), entry(entry)
{
    SetPosition(0);
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

    if (pos == entry->fileSize)
        return;

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

    DWORD bytesToRead = (len <= maxReadConsecutive) ? len : maxReadConsecutive;
    device->ReadBytes(outBuffer, bytesToRead);
    len -= bytesToRead;
    SetPosition(pos + bytesToRead);

    while (len >= entry->partition->clusterSize)
    {
        // calculate how many bytes to read
        device->ReadBytes(outBuffer + (origLen - len), maxReadConsecutive);

        // update the position
        SetPosition(pos + maxReadConsecutive);

        // update the length
        len -= maxReadConsecutive;
    }

    if (len > 0)
    {
        device->ReadBytes(outBuffer + (origLen - len), len);
    }
}

void FatxIO::WriteBytes(BYTE *buffer, DWORD len)
{
}

void FatxIO::SaveFile(std::string savePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL)
{
    /*
      Got this thing down to 20s for that one file. The only problem is that it doesn't work for
      extracting some of the profiles. It works fine for that halo map pack though, the CRCs match.
      The progress is screwed up too, I know why but I'm not sure how we'll fix that. For the profiles
      it's doing the same thing that it was before. I know that you fixed it but I'm too lazy to go
      back and figure out what you changed. I expect this to be fixed by the time that I wake up,
      if you don't, there will be consequences.
    */

    // get the current position
    INT64 pos;
    INT64 originalPos = device->GetPosition();

    // seek to the beggining of the file
    SetPosition(0);

    // open the new file
    FileIO outFile(savePath, true);

    std::vector<Range> readRanges;
    BYTE *buffer = new BYTE[0x50000];

    // generate the read ranges
    for (DWORD i = 0; i < entry->clusterChain.size(); i++)
    {
        // calculate cluster's address
        pos = (entry->partition->clusterStartingAddress + (entry->partition->clusterSize * (INT64)(entry->clusterChain.at(i) - 1)));

        Range range = { pos, 0 };
        do
        {
            range.len += entry->partition->clusterSize;
        }
        while (i < (entry->clusterChain.size() -1) && range.len < 0x50000 && (entry->clusterChain.at(i) + 1) == entry->clusterChain.at(++i));
        readRanges.push_back(range);
    }

    // read all the data in
    for (DWORD i = 0; i < readRanges.size(); i++)
    {
        // seek to the beginning of the range
        device->SetPosition(readRanges.at(i).start);

        // get the range from the device
        device->ReadBytes(buffer, readRanges.at(i).len);
        outFile.WriteBytes(buffer, readRanges.at(i).len);

        // update progress if needed
        if (progress)
            progress(arg, i + 1, readRanges.size());
    }

    outFile.Flush();
    outFile.Close();

    delete[] buffer;

    device->SetPosition(originalPos);

    // initialization
   /* BYTE *buffer = new BYTE[0x10000];
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
    device->SetPosition(pos);*/
}




















