#ifndef XCONTENTDEVICEPROFILE_H
#define XCONTENTDEVICEPROFILE_H

#include <iostream>
#include <vector>

#include "Stfs/StfsPackage.h"
#include "IO/StfsIO.h"
#include "Account/Account.h"
#include "XContentDeviceItem.h"
#include "XContentDeviceTitle.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XContentDeviceProfile : public XContentDeviceItem
{
public:
    XContentDeviceProfile(FatxFileEntry *fileEntry, StfsPackage *profile);
    XContentDeviceProfile(std::string pathOnDevice, std::string rawName, StfsPackage *profile, DWORD fileSize = 0);

    std::vector<XContentDeviceTitle> titles;

    BYTE* GetProfileID();

    virtual std::wstring GetName();

private:
    std::wstring gamertag;
};

#endif // XCONTENTDEVICEPROFILE_H
