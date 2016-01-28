#ifndef XEXBASEIO_H
#define XEXBASEIO_H

#include "BaseIO.h"
#include "Xex/Xex.h"

#define XEX_NULL_BUFFER_SIZE 	0x10000

// Abstract class containing the operations common to all XexIOs
class XexBaseIO : public BaseIO
{
public:
    XexBaseIO(BaseIO *io, Xbox360Executable *xex);

    void SetPosition(UINT64 position, std::ios_base::seek_dir dir);

    UINT64 GetPosition();

    void Flush();

    void Close();

protected:
    BaseIO *io;
    Xbox360Executable *xex;
    UINT64 position;
};

#endif // XEXBASEIO_H
