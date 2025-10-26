#ifndef FATXDRIVEDETECTION_H
#define FATXDRIVEDETECTION_H

#include <iostream>
#include <vector>
#include <sstream>
#include <XboxInternals/Fatx/FatxDrive.h>

#include <XboxInternals/Export.h>

class XBOXINTERNALS_EXPORT FatxDriveDetection
{
public:
    static std::vector<FatxDrive*> GetAllFatxDrives();

private:
    static std::vector<DeviceIO*> getPhysicalDisks();
    static std::vector<std::wstring> getLogicalDrives();
};

#endif // FATXDRIVEDETECTION_H


