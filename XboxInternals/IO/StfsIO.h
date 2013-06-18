#ifndef STFSIO_H
#define STFSIO_H

#include "BaseIO.h"
#include "Stfs/StfsPackage.h"

class XBOXINTERNALSSHARED_EXPORT StfsIO : public BaseIO
{
public:
    StfsIO(BaseIO *io, StfsPackage *package, StfsFileEntry entry);

    // flushes the package
    ~StfsIO();

    // get the current file entry
    StfsFileEntry GetStfsFileEntry();

    // set the position
    void SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg);

    // get the current position
    UINT64 GetPosition();

    // returns the length
    UINT64 Length();

    // read bytes at the current position
    void ReadBytes(BYTE *outBuffer, DWORD len);

    // Write bytes at the current position
    void WriteBytes(BYTE *buffer, DWORD len);

    // flushes the current IO, and updates the file entry information (size, blocks allocated)
    void Flush();

    // does nothing, required implementation
    void Close();

private:
    BaseIO *io;
    StfsPackage *package;
    StfsFileEntry entry;

    UINT64 entryPosition;
    bool didChangeSize;
    DWORD blocksAllocated;
};

#endif // STFSIO_H
