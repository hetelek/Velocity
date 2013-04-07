#ifndef FATXIO_H
#define FATXIO_H

#include "DeviceIO.h"
#include "FileIO.h"
#include "MemoryIO.h"
#include "../Fatx/FatxConstants.h"
#include "../Cryptography/XeCrypt.h"

#include <iostream>
#include <vector>
#include <algorithm>

struct Range
{
    UINT64 start;
    UINT64 len;
};

class XBOXINTERNALSSHARED_EXPORT FatxIO : public BaseIO
{
public:
    FatxIO(DeviceIO *device, FatxFileEntry *entry);
    virtual ~FatxIO();

    // get the current file entry
    FatxFileEntry *GetFatxFileEntry();

    // set the position
    void SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg);

    // get the current position
    UINT64 GetPosition();

    // returns the length
    UINT64 Length();

    // get the drive's current position
    UINT64 GetDrivePosition();

    // read bytes at the current position
    void ReadBytes(BYTE *outBuffer, DWORD len);

    // write bytes at the current position
    void WriteBytes(BYTE *buffer, DWORD len);

    // save the file to disk
    void SaveFile(std::string savePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // does nothing, required implementation
    void Flush();

    // does nothing, required implementation
    void Close();

    // expands the cluster chain if necessary, returns the amount of new clusters allocated
    int AllocateMemory(DWORD byteAmount);

    // write the entry to disk
    void WriteEntryToDisk(FatxFileEntry *entry, std::vector<DWORD> *clusterChain = NULL);

    // replace the file with one from a local disk
    void ReplaceFile(std::string sourcePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // convert a cluster to an offset
    static INT64 ClusterToOffset(Partition *part, DWORD cluster);

    // sets all the clusters equal to value
    static void SetAllClusters(DeviceIO *device, Partition *part, std::vector<DWORD> &clusters, DWORD value);

private:
    // find count amount of free custers
    std::vector<DWORD> getFreeClusters(Partition *part, DWORD count);

    // writes the cluster chain (and links them correctly) starting from startingCluster
    void writeClusterChain(Partition *part, DWORD startingCluster, std::vector<DWORD> clusterChain);

    // get the ranges of consecutive numbers in list where it's sorted
    void getConsecutive(std::vector<DWORD> &list, std::vector<Range> &outRanges);

    DeviceIO *device;
    FatxFileEntry *entry;
    UINT64 pos;
    DWORD maxReadConsecutive;
};

bool compareRanges(Range a, Range b);

#endif // FATXIO_H
