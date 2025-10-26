#ifndef Ytgr_H
#define Ytgr_H

#include <iostream>

#include <XboxInternals/IO/FileIO.h>
#include "../Cryptography/XeKeys.h"
#include "../Gpd/XdbfHelpers.h"
#include "../Stfs/StfsConstants.h"
#include <botan_all.h>

class XBOXINTERNALS_EXPORT Ytgr
{
public:
    Ytgr(std::string filePath);
    Ytgr(FileIO *io);
    ~Ytgr();

    DWORD magic;
    Version XSignerMinimumVersion;
    DWORD structSize;
    time_t dateAddedToServer;
    DWORD contentLength;
    DWORD reserved;
    BYTE contentHash[0x14];
    BYTE rsaSignature[0x100];
    bool valid;

private:
    FileIO *io;
    bool ioPassedIn;

    // parse Ytgr header
    void Parse();
};

#endif // Ytgr_H


