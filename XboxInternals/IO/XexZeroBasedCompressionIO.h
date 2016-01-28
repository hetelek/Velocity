#ifndef XEXZEROBASEDCOMPRESSIONIO_H
#define XEXZEROBASEDCOMPRESSIONIO_H

#include "BaseIO.h"
#include "XexBaseIO.h"
#include "Xex/Xex.h"

#define XEX_NULL_BUFFER_SIZE 	0x10000

class XexZeroBasedCompressionIO : public XexBaseIO
{
public:
    XexZeroBasedCompressionIO(BaseIO *io, Xbox360Executable *xex);

    void ReadBytes(BYTE *outBuffer, DWORD readLength);

    void WriteBytes(BYTE *buffer, DWORD len);

    UINT64 Length();
};

#endif // XEXZEROBASEDCOMPRESSIONIO_H
