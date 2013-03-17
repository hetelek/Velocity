#ifndef FATXDRIVEDETECTION_H
#define FATXDRIVEDETECTION_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include "FatxDrive.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT FatxDriveDetection
{
public:
    static std::vector<FatxDrive*> GetAllFatxDrives();

private:
    static std::vector<HANDLE> getPhysicalDisks();
    static std::vector<std::wstring> getLogicalDrives();
};

#endif // FATXDRIVEDETECTION_H
