/* Most parts of this class were originally developed by Lander Griffith (https://github.com/landr0id/).
   Much of his code is used throughout this class or very slightly modified */

#ifndef FATXDRIVE_H
#define FATXDRIVE_H

#include "FatxConstants.h"

#include "../Stfs/XContentHeader.h"
#include "../IO/DeviceIO.h"
#include "../IO/FatxIO.h"
#include "../IO/MemoryIO.h"
#include "../IO/MultiFileIO.h"
#include "../Cryptography/XeKeys.h"
#include "../Cryptography/XeCrypt.h"

#include <ctype.h>

class XBOXINTERNALSSHARED_EXPORT FatxDrive
{
public:
    FatxDrive(BaseIO *io, FatxDriveType type);
    FatxDrive(void* deviceHandle, FatxDriveType type = FatxHarddrive);
    FatxDrive(std::string drivePath, FatxDriveType type = FatxHarddrive);
    FatxDrive(std::wstring drivePath, FatxDriveType type = FatxHarddrive);
    ~FatxDrive();

    // get the drives partitions
    std::vector<Partition*> GetPartitions();

    // get the drive type
    FatxDriveType GetFatxDriveType();

    // get a FatxIO for the given entry
    FatxIO GetFatxIO(FatxFileEntry *entry);

    // populate entry's cachedFiles vector (only if it's a directory)
    void GetChildFileEntries(FatxFileEntry *entry, void(*progress)(void*, bool) = NULL, void *arg = NULL);

    // populate entry's clusterChain with its cluster chain
    void ReadClusterChain(FatxFileEntry *entry);

    // save the security blob to local disk
    void ExtractSecurityBlob(std::string path);

    // replace the existing security blob
    void ReplaceSecurityBlob(std::string path);

    // create a new file
    void CreateFileX(FatxFileEntry *parent, std::string name);

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

    // write the entire contents of the drive to the local disk
    void CreateBackup(std::string outPath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // re-write the entire contents of the drive using a backup from the local disk
    void RestoreFromBackup(std::string backupPath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    // get the amount of free bytes on the device
    UINT64 GetFreeMemory(Partition *part, void(*progress)(void*, bool) = NULL, void *arg = NULL);

    // convert a cluster to an offset
    static INT64 ClusterToOffset(Partition *part, DWORD cluster);

    // check to see whether or not a file name is valid
    static bool ValidFileName(std::string fileName);

    // format recovery version, found by Eaton (only on dev kit drives)
    Version lastFormatRecoveryVersion;

    // security sector information (only on hard drives)
    SecurityInfo securityBlob;

    // configuration data (only in flash drives)
    FlashDriveConfigurationData configurationData;

private:
    // writes the 'newEntry' to disk, in the 'parent' folder
    void createFileEntry(FatxFileEntry *parent, FatxFileEntry *newEntry);

    // open up a physical drive
    void loadFatxDrive(std::wstring drivePath);

    // open up a physical drive
    void loadFatxDrive(void* deviceHandle);

    // open up a physical drive
    void loadFatxDrive();

    // process a partition and load it with calulated information
    void processBootSector(Partition *part);

    // load all the profiles on the device
    void loadProfiles();

    // counts the largest amount of consecutive unset bits
    static BYTE cntlzw(DWORD x);

    // check to see if a certain character is allowed as a file name
    static bool validFileChar(char c);

    BaseIO *io;
    std::vector<Partition*> partitions;
    std::vector<FatxFileEntry*> profiles;
    FatxDriveType type;
};

#endif // FATXDRIVE_H
