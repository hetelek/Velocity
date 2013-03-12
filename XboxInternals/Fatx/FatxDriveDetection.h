#ifndef FATXDRIVEDETECTION_H
#define FATXDRIVEDETECTION_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include "FatxDrive.h"

#include <QDebug>

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT FatxDriveDetection
{
public:
    static std::vector<FatxDrive*> GetAllFatxDrives();

private:
    static std::vector<HANDLE> getPhysicalDisks();
};

#endif // FATXDRIVEDETECTION_H
