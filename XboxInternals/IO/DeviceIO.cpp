#include "DeviceIO.h"

DeviceIO::DeviceIO(std::string devicePath)
{
    // convert it to a wstring
    std::wstring wsDevicePath;
    wsDevicePath.assign(devicePath.begin(), devicePath.end());

    // load the device
    loadDevice(wsDevicePath);
}

DeviceIO::DeviceIO(std::wstring devicePath)
{
    // load the device
    loadDevice(devicePath);
}

void DeviceIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    #ifdef _WIN32
        if (deviceHandle == INVALID_HANDLE_VALUE)
            throw std::string("DeviceIO: INVALID_HANDLE_VALUE.\n");
    #endif

    // don't let the count exceed the drive length
    if (pos + len > DriveLength())
        len = (int)((pos >= DriveLength()) ? 0 : DriveLength() - pos);

    // nothing to do
    if (len == 0)
        return;

    BYTE maxSectors = (BYTE)(UP_TO_NEAREST_SECTOR(len + (pos - realPosition())) / 0x200); // This is the number of sectors we have to read
    int bytesToShaveOffBeginning = (int)(pos - realPosition());	// Number of bytes to remove from the beginning of the buffer
    int bytesToShaveOffEnd = (int)(UP_TO_NEAREST_SECTOR(pos + len) - (pos + len));
    int bytesThatAreInLastDataRead = 0x200 - bytesToShaveOffBeginning;

    int allDataLength = bytesToShaveOffBeginning + len + bytesToShaveOffEnd;

    if (maxSectors != allDataLength / 0x200)
        throw std::string("DeviceIO: MaxSectors != AllDataLength / 0x200.\n");

    BYTE *allData = 0;
    DWORD bytesRead;

    // If the last time we cached data wasn't at this offset
    if (lastReadOffset != realPosition())
    {
        // Cache
        #ifdef _WIN32
                ReadFile(
                    deviceHandle,	// Device to read from
                    lastReadData,	// Output buffer
                    0x200,			// Read the last sector
                    &bytesRead,		// Pointer to the number of bytes read
                    &offset);		// OVERLAPPED structure containing the offset to read from
        #else
                read(device, lastReadData, 0x200);
        #endif
        lastReadOffset = realPosition();
    }

    if (bytesThatAreInLastDataRead <= len)
        SetPosition(pos + bytesThatAreInLastDataRead);
    else
        SetPosition(pos + len);

    if (maxSectors > 1)
    {
        allData = new BYTE[allDataLength - 0x200];

        // Read for all sectors EXCEPT the last one
        #ifdef _WIN32
                ReadFile(
                    deviceHandle,	// Device to read from
                    allData,		// Output buffer
                    allDataLength - 0x200,	// Read the last sector
                    &bytesRead,		// Pointer to the number of bytes read
                    &offset);		// OVERLAPPED structure containing the offset to read from
        #else
                read(device, allData, allDataLength - 0x200);
        #endif

        SetPosition(pos + (len - bytesThatAreInLastDataRead));
    }

    int countRead = ((bytesThatAreInLastDataRead <= len) ? bytesThatAreInLastDataRead : len);
    memcpy(outBuffer, lastReadData + bytesToShaveOffBeginning, countRead);
    if (allData)
    {
        memcpy(outBuffer + bytesThatAreInLastDataRead, allData, len - bytesThatAreInLastDataRead);

        // Cache
        memcpy(&lastReadData, allData + allDataLength - ((0x200 * 2)), 0x200);
        delete[] allData;
    }
}

void DeviceIO::WriteBytes(BYTE *buffer, DWORD len)
{
    #ifdef _WIN32
        if (deviceHandle == INVALID_HANDLE_VALUE)
            throw std::string("DeviceIO: INVALID_HANDLE_VALUE.");
    #endif

    // We can't write beyond the end of the stream
    if (pos + len > DriveLength())
        throw std::string("Can not write beyond end of stream! At xDeviceStream::Write");

    // nothing to do
    if (len == 0)
        return;

    UINT64 originalPos = pos;
    UINT64 finalPos = pos + len;
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

    // set the position
    SetPosition(finalPos);
}

UINT64 DeviceIO::DriveLength()
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
