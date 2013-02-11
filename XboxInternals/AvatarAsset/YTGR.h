#ifndef YTGR_H
#define YTGR_H

#include <iostream>

#include "FileIO.h"
#include "../Cryptography/XeKeys.h"
#include "../GPD/XDBFHelpers.h"

class XBOXINTERNALSSHARED_EXPORT YTGR
{
public:
    YTGR(std::string filePath);
    YTGR(FileIO *io);

    DWORD magic;
    DWORD XSignerMinimumVersion;
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

    // parse YTGR header
    void Parse();
};

#endif // YTGR_H
