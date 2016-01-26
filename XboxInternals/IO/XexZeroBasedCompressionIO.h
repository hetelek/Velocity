#ifndef XEXZEROBASEDCOMPRESSIONIO_H
#define XEXZEROBASEDCOMPRESSIONIO_H

#include "BaseIO.h"
#include "Xex/Xex.h"

#define XEX_NULL_BUFFER_SIZE 	0x10000

class XexZeroBasedCompressionIO : public BaseIO
{
public:
    XexZeroBasedCompressionIO(BaseIO *io, Xbox360Executable *xex);

    void ReadBytes(BYTE *outBuffer, DWORD readLength);

    void WriteBytes(BYTE *buffer, DWORD len);

    void SetPosition(UINT64 position, std::ios_base::seek_dir dir);

    UINT64 GetPosition();

    void Flush();

    void Close();

    UINT64 Length();

private:
    Xbox360Executable *xex;
    BaseIO *io;
    UINT64 position;
};

#endif // XEXZEROBASEDCOMPRESSIONIO_H
