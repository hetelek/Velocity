#include "FatxDriveDetection.h"
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>

#ifdef __APPLE__
#include <sys/disk.h>
#include <sys/ioctl.h>
#endif

#ifdef __linux__
#include <mntent.h>
#endif
#endif

std::vector<FatxDrive*> FatxDriveDetection::GetAllFatxDrives()
{
    std::vector<std::wstring> logicalDrivePaths = getLogicalDrives();
    std::vector<FatxDrive*> drives;

    std::vector<DeviceIO*> devices = getPhysicalDisks();

    for (size_t i = 0; i < devices.size(); i++)
    {
        if (devices.at(i)->Length() > HddOffsets::Data)
        {
            devices.at(i)->SetPosition(HddOffsets::Data);
            if (devices.at(i)->ReadDword() == FATX_MAGIC)
            {
                FatxDrive *drive = new FatxDrive(static_cast<BaseIO*>(devices.at(i)), FatxHarddrive);
                drives.push_back(drive);
            }
            else
                devices.at(i)->Close();
        }
    }

    std::vector<std::string> dataFiles;
    for (size_t i = 0; i < logicalDrivePaths.size(); i++)
    {
        // clear data files from the previous drive
        dataFiles.clear();

        std::string directory;
        directory.assign(logicalDrivePaths.at(i).begin(), logicalDrivePaths.at(i).end());

        // Modern C++20: Use std::filesystem instead of FindFirstFile/readdir
        std::filesystem::path dirPath(directory);
        
        try
        {
            if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath))
            {
                for (const auto& entry : std::filesystem::directory_iterator(dirPath))
                {
                    if (entry.is_regular_file())
                    {
                        std::string filename = entry.path().filename().string();
                        // Check if filename starts with "Data"
                        if (filename.size() >= 4 && filename.substr(0, 4) == "Data")
                        {
                            dataFiles.push_back(entry.path().string());
                        }
                    }
                }

                if (dataFiles.size() >= 3)
                {
                    // make sure the data files are loaded in the right order
                    std::sort(dataFiles.begin(), dataFiles.end());
                    MultiFileIO *io = new MultiFileIO(dataFiles);
                    FatxDrive *usbDrive = new FatxDrive(io, FatxFlashDrive);
                    drives.push_back(usbDrive);
                }
            }
        }
        catch (const std::filesystem::filesystem_error&)
        {
            // Directory doesn't exist or can't be accessed, skip it
            continue;
        }
    }

    return drives;
}

std::vector<DeviceIO*> FatxDriveDetection::getPhysicalDisks()
{
    std::vector<DeviceIO*> physicalDiskPaths;
    std::wstringstream ss;

#ifdef _WIN32
    for (int i = 0; i < 16; i++)
    {
        ss << L"\\\\.\\PHYSICALDRIVE" << i;

        HANDLE drive = CreateFile(ss.str().c_str(), GENERIC_READ|GENERIC_WRITE,
                FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (drive != INVALID_HANDLE_VALUE)
        {
            CloseHandle(drive);

            DeviceIO *io = new DeviceIO(ss.str());
            physicalDiskPaths.push_back(io);
        }

        ss.str(std::wstring());
    }
#else
    // Modern C++20: Use std::filesystem for /dev/ enumeration
    std::filesystem::path devPath("/dev/");
    
    try
    {
        if (std::filesystem::exists(devPath) && std::filesystem::is_directory(devPath))
        {
            for (const auto& entry : std::filesystem::directory_iterator(devPath))
            {
                std::string deviceName = entry.path().filename().string();
                bool isValidDevice = false;
                std::string devicePath;

#ifdef __APPLE__
                // macOS: Look for disk* devices (use raw device rdisk*)
                if (deviceName.size() >= 4 && deviceName.substr(0, 4) == "disk")
                {
                    devicePath = "/dev/r" + deviceName;  // Use raw device
                    isValidDevice = true;
                }
#elif __linux__
                // Linux: Look for sd* devices (sda, sdb, etc.)
                if (deviceName.size() >= 2 && deviceName.substr(0, 2) == "sd")
                {
                    devicePath = entry.path().string();
                    isValidDevice = true;
                }
#endif

                if (isValidDevice)
                {
                    // Try to open the device to verify access
                    int device = open(devicePath.c_str(), O_RDWR);
                    if (device > 0)
                    {
                        close(device);
                        DeviceIO *io = new DeviceIO(devicePath);
                        physicalDiskPaths.push_back(io);
                    }
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        // /dev/ not accessible, return empty list
    }
#endif

    return physicalDiskPaths;
}

std::vector<std::wstring> FatxDriveDetection::getLogicalDrives()
{
    std::vector<std::wstring> driveStrings;

#ifdef _WIN32
    DWORD drives = GetLogicalDrives();
    std::wstringstream ss;
    char currentChar = 'A';

    for (int i = 0; i < 32; i++)
    {
        if (drives & 1)
        {
            ss << currentChar << ":\\Xbox360";
            driveStrings.push_back(ss.str());
            ss.str(std::wstring());
        }

        drives >>= 1;
        currentChar++;
    }
#elif __APPLE__
    // Modern C++20: Use std::filesystem for /Volumes/ enumeration
    std::filesystem::path volumesPath("/Volumes/");
    
    try
    {
        if (std::filesystem::exists(volumesPath) && std::filesystem::is_directory(volumesPath))
        {
            for (const auto& entry : std::filesystem::directory_iterator(volumesPath))
            {
                if (entry.is_directory())
                {
                    try
                    {
                        // Check for Xbox360 subdirectory
                        std::filesystem::path xboxPath = entry.path() / "Xbox360";
                        
                        if (std::filesystem::exists(xboxPath) && std::filesystem::is_directory(xboxPath))
                        {
                            std::string xboxDirPath = xboxPath.string() + "/";
                            std::wstring widePathStr;
                            widePathStr.assign(xboxDirPath.begin(), xboxDirPath.end());
                            driveStrings.push_back(widePathStr);
                        }
                    }
                    catch (const std::filesystem::filesystem_error&)
                    {
                        // This volume doesn't have Xbox360 folder or can't be accessed, skip it
                        continue;
                    }
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error&)
    {
        // /Volumes/ not accessible
    }
#elif __linux__
    // Linux: Parse /proc/mounts (must use platform API)
    struct mntent *ent;
    FILE *mounts = setmntent("/proc/mounts", "r");

    if (mounts != NULL)
    {
        while ((ent = getmntent(mounts)) != NULL)
        {
            try
            {
                // Check for Xbox360 subdirectory in this mount point
                std::filesystem::path xboxPath = std::filesystem::path(ent->mnt_dir) / "Xbox360";
                
                if (std::filesystem::exists(xboxPath) && std::filesystem::is_directory(xboxPath))
                {
                    std::string xboxDirPath = xboxPath.string() + "/";
                    std::wstring widePathStr;
                    widePathStr.assign(xboxDirPath.begin(), xboxDirPath.end());
                    driveStrings.push_back(widePathStr);
                }
            }
            catch (const std::filesystem::filesystem_error&)
            {
                // This mount point doesn't have Xbox360 or can't be accessed, skip it
                continue;
            }
        }

        endmntent(mounts);
    }
#endif
    return driveStrings;
}


