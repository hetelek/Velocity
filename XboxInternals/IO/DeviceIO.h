/* Most parts of this class were originally developed by Lander Griffith (https://github.com/landr0id/).
   Much of his code is used throughout this class or very slightly modified */

#ifndef DEVICEIO_H
#define DEVICEIO_H

#include "BaseIO.h"
#include "../Fatx/FatxHelpers.h"
#include "XboxInternals_global.h"

#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <windows.h>
    #include <WinIoCtl.h>
#else
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #if __APPLE__
        #include <sys/disk.h>
    #elif __linux
        #include <linux/fs.h>
    #endif
    #include <unistd.h>
#endif

#ifdef __linux
    #define SECTOR_COUNT BLKGETSIZE
    #define SECTOR_SIZE BLKSSZGET
#elif __APPLE__
    #define SECTOR_COUNT DKIOCGETBLOCKCOUNT
    #define SECTOR_SIZE DKIOCGETBLOCKSIZE
#endif


class XBOXINTERNALSSHARED_EXPORT DeviceIO : public BaseIO
{
public:
    DeviceIO(void* deviceHandle);
    DeviceIO(std::string devicePath);
    DeviceIO(std::wstring devicePath);
    virtual ~DeviceIO();

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    void SetPosition(UINT64 address, std::ios_base::seek_dir dir = std::ios_base::beg);

    UINT64 GetPosition();

    UINT64 Length();

    void Close();

    void Flush();

private:
    void loadDevice(std::wstring devicePath);

    UINT64 realPosition();

    class Impl;
    Impl* impl;

    std::string yolo;

    UINT64 pos;
    BYTE lastReadData[0x200];
    UINT64 lastReadOffset;
};

#endif // DEVICEIO_H
