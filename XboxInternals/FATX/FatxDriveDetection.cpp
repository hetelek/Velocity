#include "FatxDriveDetection.h"
#include <Windows.h>
#include <algorithm>

std::vector<FatxDrive*> FatxDriveDetection::GetAllFatxDrives()
{
    std::vector<std::wstring> logicalDrivePaths = getLogicalDrives();
    std::vector<FatxDrive*> drives;
    std::vector<HANDLE> devices = getPhysicalDisks();

    for (int i = 0; i < devices.size(); i++)
    {
        try
        {
            DeviceIO io(devices.at(i));
            if (io.Length() > HddOffsets::Data)
            {
                io.SetPosition(HddOffsets::Data);
                if (io.ReadDword() == FATX_MAGIC)
                {
                    FatxDrive *drive = new FatxDrive(devices.at(i));
                    drives.push_back(drive);
                }
                else
                    io.Close();
            }
        }
        catch (...)
        {
        }
    }

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

    return drives;
}

std::vector<void*> FatxDriveDetection::getPhysicalDisks()
{
    std::vector<void*> physicalDiskPaths;
    std::wstringstream ss;

    for (int i = 0; i < 16; i++)
    {
        ss << L"\\\\.\\PHYSICALDRIVE" << i;

        HANDLE drive = CreateFile(ss.str().c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (drive != INVALID_HANDLE_VALUE)
            physicalDiskPaths.push_back((void*)drive);

        ss.str(std::wstring());
    }

    return physicalDiskPaths;
}

std::vector<std::wstring> FatxDriveDetection::getLogicalDrives()
{
    DWORD drives = GetLogicalDrives();
    std::vector<std::wstring> driveStrings;
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

    return driveStrings;
}
