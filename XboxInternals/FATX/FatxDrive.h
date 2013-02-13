#ifndef FATXDRIVE_H
#define FATXDRIVE_H

#include <iostream>

#include "../IO/DeviceIO.h"
#include "../Cryptography/XeKeys.h"

struct SecurityInfo
{
    std::string serialNumber;
    std::string firmwareRevision;
    std::string modelNumber;
    BYTE msLogoHash[0x14];
    DWORD userAddressableSectors;
    BYTE rsaSignature[0x100];
    bool validSignature;

    DWORD msLogoSize;
    BYTE *msLogo;
};

class XBOXINTERNALSSHARED_EXPORT FatxDrive
{
public:
    FatxDrive(std::wstring drivePath);
    ~FatxDrive();

    SecurityInfo securityBlob;

private:
    DeviceIO *io;
};

#endif // FATXDRIVE_H
