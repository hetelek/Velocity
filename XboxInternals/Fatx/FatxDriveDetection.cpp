#include "FatxDriveDetection.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <dirent.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>

    #ifdef __APPLE__
        #include <sys/disk.h>
        #include <sys/ioctl.h>
    #endif

    #ifdef __linux
        #include <linux/hdreg.h>
        #include <pwd.h>
    #endif
#endif

std::vector<FatxDrive*> FatxDriveDetection::GetAllFatxDrives()
{
    std::vector<std::wstring> logicalDrivePaths = getLogicalDrives();
    std::vector<FatxDrive*> drives;

    getPhysicalDisks();


    std::vector<DeviceIO*> devices = getPhysicalDisks();

    for (int i = 0; i < devices.size(); i++)
    {
        try
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
        catch (...)
        {
        }
    }

#ifdef _WIN32
    for (int i = 0; i < logicalDrivePaths.size(); i++)
    {
        try
        {
            std::vector<std::string> dataFiles;
            WIN32_FIND_DATA fi;

            HANDLE h = FindFirstFile((logicalDrivePaths.at(i) + L"\\Data*").c_str(), &fi);
            if (h != INVALID_HANDLE_VALUE)
            {
                std::string directory;
                directory.assign(logicalDrivePaths.at(i).begin(), logicalDrivePaths.at(i).end());

                dataFiles.clear();
                do
                {
                    char path[9];
                    wcstombs(path, fi.cFileName, wcslen(fi.cFileName) + 1);
                    dataFiles.push_back(directory + "\\" + std::string(path));
                }
                while (FindNextFile(h, &fi));

                FindClose(h);

                if (dataFiles.size() >= 3)
                {
                    // Make sure the data files are loaded in the right order
                    std::sort(dataFiles.begin(), dataFiles.end());
                    MultiFileIO *io = new MultiFileIO(dataFiles);
                    FatxDrive *usbDrive = new FatxDrive(io, FatxFlashDrive);
                    drives.push_back(usbDrive);
                }
            }
        }
        catch (...)
        {
        }
    }

#endif

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

        HANDLE drive = CreateFile(ss.str().c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (drive != INVALID_HANDLE_VALUE)
        {
            CloseHandle(drive);

            DeviceIO *io = new DeviceIO(ss.str());
            physicalDiskPaths.push_back(io);
        }

        ss.str(std::wstring());
    }
#else
    DIR *dir = NULL;
    dirent *ent = NULL;
    dir = opendir("/dev/");
    if (dir != NULL)
    {
        // search for valid drives
        while ((ent = readdir(dir)) != NULL)
        {
            // the disks start with 'sd'
            if (std::string(ent->d_name).substr(0, 2) == "sd")
            {
                std::ostringstream ss;
                ss << "/dev/";
                ss << ent->d_name;
                std::string diskPath = ss.str();

                int device;
                if ((device = open(diskPath.c_str(), O_RDWR)) > 0)
                {
                    close(device);

                    DeviceIO *io = new DeviceIO(diskPath);
                    physicalDiskPaths.push_back(io);
                }
            }
        }
    }
    if (dir)
        closedir(dir);
    if (ent)
        delete ent;
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
#endif
    return driveStrings;
}
