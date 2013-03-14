/* Most parts of this class were originally developed by Lander Griffith (https://github.com/landr0id/).
   Much of his code is used throughout this class or very slightly modified */

#ifndef FATXDRIVE_H
#define FATXDRIVE_H

#include "../Stfs/StfsDefinitions.h"
#include "../Stfs/StfsConstants.h"

#include "../IO/DeviceIO.h"
#include "../IO/FatxIO.h"
#include "../IO/MemoryIO.h"
#include "../Cryptography/XeKeys.h"
#include "FatxConstants.h"

class XBOXINTERNALSSHARED_EXPORT FatxDrive
{

public:
    FatxDrive(HANDLE deviceHandle);
    FatxDrive(std::string drivePath);
    FatxDrive(std::wstring drivePath);
    ~FatxDrive();

    // format recovery version, found by Eaton
    Version lastFormatRecoveryVersion;

    // get the drives partitions
    std::vector<Partition*> GetPartitions();

    // get a FatxIO for the given entry
    FatxIO GetFatxIO(FatxFileEntry *entry);

    // populate entry's cachedFiles vector (only if it's a directory)
    void GetChildFileEntries(FatxFileEntry *entry, void(*progress)(void*, bool) = NULL, void *arg = NULL);

    // populate entry's clusterChain with its cluster chain
    void ReadClusterChain(FatxFileEntry *entry);

    // save the security blob to local disk
    void ExtractSecurityBlob(std::string path);

    // writes the a folder named 'folderName', in the 'parent' folder
    void CreateFolder(FatxFileEntry *parent, std::string folderName);

    // get the first 4 bytes of a file
    void GetFileEntryMagic(FatxFileEntry *entry);

    // deletes the entry and all of it's children
    void DeleteFile(FatxFileEntry *entry);

    // inject the file
    void InjectFile(FatxFileEntry *parent, std::string name, std::string filePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // determines if a file at the specified path exists
    bool FileExists(std::string filePath);

    // determines if a file in the specified folder exists
    bool FileExists(FatxFileEntry *folder, std::string fileName, bool checkDeleted = false);

    // get the FatxFileEntry from its path
    FatxFileEntry* GetFileEntry(std::string filePath);

    // close the underlying io
    void Close();

    // get the amount of free bytes on the device
    UINT64 GetFreeMemory(Partition *part, void(*progress)(void*, bool) = NULL, void *arg = NULL);

    // convert a cluster to an offset
    static INT64 ClusterToOffset(Partition *part, DWORD cluster);

    SecurityInfo securityBlob;

private:
    // writes the 'newEntry' to disk, in the 'parent' folder
    void createFileEntry(FatxFileEntry *parent, FatxFileEntry *newEntry);

    // open up a physical drive
    void loadFatxDrive(std::wstring drivePath);

    // open up a physical drive
    void loadFatxDrive(HANDLE deviceHandle);

    // open up a physical drive
    void loadFatxDrive();

    // process a partition and load it with calulated information
    void processBootSector(Partition *part);

    // load all the profiles on the device
    void loadProfiles();

    // counts the largest amount of consecutive unset bits
    static BYTE cntlzw(DWORD x);

    BaseIO *io;
    std::vector<Partition*> partitions;
    std::vector<FatxFileEntry*> profiles;
};

#endif // FATXDRIVE_H
