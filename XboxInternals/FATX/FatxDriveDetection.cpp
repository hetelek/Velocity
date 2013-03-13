#include "FatxDriveDetection.h"

std::vector<FatxDrive*> FatxDriveDetection::GetAllFatxDrives()
{
    std::vector<FatxDrive*> drives;
    std::vector<HANDLE> devices = getPhysicalDisks();

    for (int i = 0; i < devices.size(); i++)
    {
        try
        {
            DeviceIO io(devices.at(i));
            if (io.DriveLength() > HddOffsets::Data)
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

    return drives;
}

std::vector<HANDLE> FatxDriveDetection::getPhysicalDisks()
{
    std::vector<HANDLE> physicalDiskPaths;
    std::wstringstream ss;

    for (int i = 0; i < 16; i++)
    {
        ss << L"\\\\.\\PHYSICALDRIVE" << i;

        HANDLE drive = CreateFile(ss.str().c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (drive != INVALID_HANDLE_VALUE)
            physicalDiskPaths.push_back(drive);

        ss.str(std::wstring());
    }

    return physicalDiskPaths;
}
