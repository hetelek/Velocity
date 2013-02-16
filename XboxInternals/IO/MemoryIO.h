#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include "winnames.h"
#include <string.h>
#include "BaseIO.h"

class XBOXINTERNALSSHARED_EXPORT MemoryIO : public BaseIO
{
public:
    MemoryIO(BYTE *data, size_t length);

    void SetPosition(UINT64 pos, std::ios_base::seek_dir dir = std::ios_base::beg);
    UINT64 GetPosition();

    void ReadBytes(BYTE *outBuffer, DWORD len);
    void WriteBytes(BYTE *buffer, DWORD len);

private:
    BYTE *memory;
    size_t length;

    DWORD pos;
};

#endif // MEMORYSTREAM_H
