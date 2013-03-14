#include "DeviceIO.h"


DeviceIO::DeviceIO(HANDLE deviceHandle) :
    lastReadOffset(-1)
{
    #ifdef __WIN32
        pos = 0;
        if (deviceHandle == INVALID_HANDLE_VALUE)
            throw std::string("DeviceIO: Invalid device handle.\n");

        this->deviceHandle = deviceHandle;
        memset(&offset, 0, sizeof(OVERLAPPED));
    #endif
}

DeviceIO::DeviceIO(std::string devicePath) :
    lastReadOffset(-1)
{
    // convert it to a wstring
    std::wstring wsDevicePath;
    wsDevicePath.assign(devicePath.begin(), devicePath.end());

    // load the device
    loadDevice(wsDevicePath);
}

DeviceIO::DeviceIO(std::wstring devicePath) :
    lastReadOffset(-1)
{
    // load the device
    loadDevice(devicePath);
}

DeviceIO::~DeviceIO()
{

}

void DeviceIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    UINT64 endingPos = pos + len;
    if ((pos & 0x1FF) == 0 && (len & 0x1FF) == 0)
    {
        #ifdef _WIN32
            bool success = ReadFile(
                deviceHandle,	// Device to read from
                outBuffer,      // Output buffer
                len,			// Length to read
                NULL,           // Pointer to the number of bytes read
                &offset);		// OVERLAPPED structure containing the offset to read from

            if (!success)
                throw std::string("DeviceIO: Error reading from device, may be disconnected.\n");
        #else
            read(device, lastReadData, 0x200);
        #endif

        SetPosition(endingPos);

        return;
    }

    UINT64 originalPos = pos;
    SetPosition(DOWN_TO_NEAREST_SECTOR(pos));

    if (lastReadOffset != pos)
    {
        #ifdef _WIN32
                bool success = ReadFile(
                    deviceHandle,	// Device to read from
                    lastReadData,	// Output buffer
                    0x200,			// Length to read
                    NULL,           // Pointer to the number of bytes read
                    &offset);		// OVERLAPPED structure containing the offset to read from

                if (!success)
                    throw std::string("DeviceIO: Error reading from device, may be disconnected.\n");
        #else
                read(device, lastReadData, 0x200);
        #endif
    }
    lastReadOffset = pos;

    // copy over the data requested
    WORD bytesLeftInSector = 0x200 - (originalPos & 0x1FF);
    WORD neededBytes = (len > bytesLeftInSector) ? bytesLeftInSector : len;
    memcpy(outBuffer, lastReadData + (originalPos & 0x1FF), neededBytes);

    len -= neededBytes;
    outBuffer += neededBytes;

    if (len == 0)
    {
        SetPosition(endingPos);
        return;
    }

    // seek to the next sector
    SetPosition(pos + 0x200); // 0x200

    INT64 downTo = DOWN_TO_NEAREST_SECTOR(len);

    // read the consecutive sectors
    #ifdef _WIN32
            bool success = ReadFile(
                deviceHandle,                       // Device to read from
                outBuffer,                          // Output buffer
                downTo,                             // Length to read
                NULL,                               // Pointer to the number of bytes read
                &offset);                           // OVERLAPPED structure containing the offset to read from

            if (!success)
                throw std::string("DeviceIO: Error reading from device, may be disconnected.\n");
    #else
            read(device, lastReadData, 0x200);
    #endif

    // update all our values
    SetPosition(DOWN_TO_NEAREST_SECTOR(endingPos));
    outBuffer += downTo;
    len &= 0x1FF;

    if (len == 0)
    {
        SetPosition(endingPos);
        return;
    }

    // read the stragglers
    #ifdef _WIN32
            success = ReadFile(
                deviceHandle,                       // Device to read from
                lastReadData,                       // Output buffer
                0x200,                              // Length to read
                NULL,                               // Pointer to the number of bytes read
                &offset);                           // OVERLAPPED structure containing the offset to read from

            if (!success)
                throw std::string("DeviceIO: Error reading from device, may be disconnected.\n");
    #else
            read(device, lastReadData, 0x200);
    #endif

    // copy over the requested data
    memcpy(outBuffer, lastReadData, len);

    SetPosition(endingPos);
}

void DeviceIO::WriteBytes(BYTE *buffer, DWORD len)
{
    UINT64 endingPos = pos + len;
    if ((pos & 0x1FF) == 0 && (len & 0x1FF) == 0)
    {
        #ifdef _WIN32
            bool success = WriteFile(
                deviceHandle,	// Device to read from
                buffer,      // Output buffer
                len,			// Length to read
                NULL,           // Pointer to the number of bytes read
                &offset);		// OVERLAPPED structure containing the offset to read from

            if (!success)
                throw std::string("DeviceIO: Error writing to the device, may be disconnected.\n");
        #else
            write(device, lastReadData, 0x200);
        #endif

        SetPosition(endingPos);

        return;
    }

    // nothing to do
    if (len == 0)
        return;

    // We can't write beyond the end of the stream
    if (pos + len > Length())
        throw std::string("Can not write beyond end of stream! At xDeviceStream::Write");


    UINT64 originalPos = pos;
    UINT64 currentSector = DOWN_TO_NEAREST_SECTOR(pos);

    // write the bytes up to the next sector
    SetPosition(currentSector);
    ReadBytes(lastReadData, 0x200);
    WORD bytesLeftInSector = 0x200 - (originalPos - currentSector);
    WORD bytesToWrite = (len >= bytesLeftInSector) ? bytesLeftInSector : len;
    memcpy(lastReadData + (originalPos - currentSector), buffer, bytesToWrite);

    // write the actual data
    DWORD bytesWritten;
    SetPosition(currentSector);
    #ifdef _WIN32
        WriteFile(
            deviceHandle,		// Device to read from
            lastReadData,       // Data to write
            0x200,              // Amount of data to write
            &bytesWritten,      // Pointer to number of bytes written
            &offset);			// OVERLAPPED structure containing the offset to write from
    #else
        write(device, lastReadData, len);
    #endif

    // update the values
    buffer += bytesToWrite;
    currentSector += 0x200;
    len -= bytesToWrite;

    // write the rest of the data
    while (len > 0)
    {
        SetPosition(currentSector);
        ReadBytes(lastReadData, 0x200);
        bytesToWrite = (len >= 0x200) ? 0x200 : len;
        memcpy(lastReadData, buffer, bytesToWrite);

        // go to the top of the sector
        SetPosition(currentSector);

        // update values
        buffer += bytesToWrite;
        currentSector += 0x200;
        len -= bytesToWrite;

        #ifdef _WIN32
            WriteFile(
                deviceHandle,		// Device to read from
                lastReadData,       // Data to write
                0x200,              // Amount of data to write
                &bytesWritten,      // Pointer to number of bytes written
                &offset);			// OVERLAPPED structure containing the offset to write from
        #else
            write(device, buffer, len);
        #endif
    }

    lastReadOffset = (pos == 0) ? 0 : pos - 0x200;

    // set the position
    SetPosition(endingPos);
}

UINT64 DeviceIO::Length()
{
    UINT64 length;
    #ifdef _WIN32
        DISK_GEOMETRY geometry;
        DWORD bytesReturned;

        memset(&geometry, 0, sizeof(DISK_GEOMETRY));
        DeviceIoControl(
            deviceHandle,					// Device to get the geometry from
            IOCTL_DISK_GET_DRIVE_GEOMETRY,	// Action we're taking (getting the geometry)
            NULL,							// Not used since this requires no input data
            0,								// No data
            &geometry,						// Output struct
            sizeof(DISK_GEOMETRY),			// Output buffer size
            &bytesReturned,					// Number of bytes returned
            NULL);							// Not used

        UINT64 cylinders = (UINT64)((UINT64)geometry.Cylinders.HighPart << 32) | geometry.Cylinders.LowPart; // Convert the BIG_INTEGER to UINT64
        length = cylinders * (UINT64)geometry.TracksPerCylinder	* (UINT64)geometry.SectorsPerTrack * (UINT64)geometry.BytesPerSector;
    #else
        DWORD *numberOfSectors = new DWORD;
        *numberOfSectors = 0;
        int _device = device;

        // Queue number of sectors
        ioctl(_device, DKIOCGETBLOCKCOUNT, numberOfSectors);

        DWORD *sectorSize = new DWORD;
        *sectorSize = 0;
        ioctl(device, DKIOCGETBLOCKSIZE, sectorSize);

        qDebug("#S: 0x%X, SS: 0x%X", *numberOfSectors, *sectorSize);

        length = (UINT64)*numberOfSectors * (UINT64)*sectorSize;

        delete numberOfSectors;
        delete sectorSize;
    #endif

        return length;
}

void DeviceIO::SetPosition(UINT64 address, std::ios_base::seek_dir dir)
{
    if (dir != std::ios_base::beg)
        throw std::string("DeviceIO: Unsupported seek direction\n");

    pos = address;
    address = DOWN_TO_NEAREST_SECTOR(address); // Round the position down to the nearest sector offset

    #ifdef _WIN32
        offset.Offset = (DWORD)address;
        offset.OffsetHigh = (DWORD)(address >> 32);
    #else
        offset = address;
        lseek(device, address, SEEK_SET);
    #endif
}

UINT64 DeviceIO::realPosition()
{
    #ifdef _WIN32
        return (UINT64)(((UINT64)offset.OffsetHigh << 32) | offset.Offset);
    #else
        return offset;
    #endif
}

UINT64 DeviceIO::GetPosition()
{
    return pos;
}

void DeviceIO::Close()
{
    #if defined _WIN32
        if (deviceHandle != INVALID_HANDLE_VALUE)
            CloseHandle(deviceHandle);
    #else
        close(device);
        device = NULL;
    #endif
}

void DeviceIO::loadDevice(std::wstring devicePath)
{
    #ifdef _WIN32
        pos = 0;
        memset(&offset, 0, sizeof(OVERLAPPED));

        // Attempt to get a handle to the device
        deviceHandle = CreateFile(
                devicePath.c_str(),// File name (device path)
                GENERIC_READ | GENERIC_WRITE,		// Read/write access
                FILE_SHARE_READ | FILE_SHARE_WRITE,	// Read/write share
                NULL,								// Not used
                OPEN_EXISTING,						// Open the existing device -- fails if it's fucked
                FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_DEVICE,	// Flags and attributes
                NULL);								// Ignored

        if (deviceHandle == INVALID_HANDLE_VALUE)
            throw std::string("DeviceIO: Could not open HANDLE for device.\n");
    #else
        // Open the device
        device = open(devicePath.c_str(), O_RDWR);
        if (device == -1)
            throw std::string("DeviceIO: Error opening device.\n");
    #endif
}

void DeviceIO::Flush()
{
    #ifdef __WIN32
        FlushFileBuffers(deviceHandle);
    #endif
}
