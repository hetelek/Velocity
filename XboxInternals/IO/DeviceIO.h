/* Most parts of this class were originally developed by Lander Griffith (https://github.com/landr0id/).
   Much of his code is used throughout this class */

#ifndef SVODIO_H
#define SVODIO_H

#include "../Disc/gdfx.h"
#include "BaseIO.h"
#include "../Stfs/XContentHeader.h"
#include "XboxInternals_global.h"

#ifdef _WIN32
    #include <windows.h>
    #include <WinIoCtl.h>
#else
#define _FILE_OFFSET_BITS 64
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <sys/disk.h>
    #include <unistd.h>
#endif

#ifndef _WIN32
    #include <sys/stat.h>
#endif


class XBOXINTERNALSSHARED_EXPORT DeviceIO : public BaseIO
{
public:
    DeviceIO();

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    void SetPosition(DWORD address);

    UINT64 DriveLength();

    void Close();

private:
    #ifdef _WIN32
        HANDLE deviceHandle;
        OVERLAPPED offset;
    #else
        int device;
        INT64 offset;
    #endif

    INT64 userOffset;
    INT64 realPosition();
};

#endif // SVODIO_H
