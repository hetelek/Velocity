#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <XboxInternals/TypeDefinitions.h>
#include <string.h>
#include <XboxInternals/IO/BaseIO.h>

class XBOXINTERNALS_EXPORT MemoryIO : public BaseIO
{
public:
    MemoryIO(BYTE *data, size_t length);
    virtual ~MemoryIO();

    void SetPosition(UINT64 pos, std::ios_base::seekdir dir = std::ios_base::beg);
    UINT64 GetPosition();
    UINT64 Length();

    void ReadBytes(BYTE *outBuffer, DWORD len);
    void WriteBytes(BYTE *buffer, DWORD len);

    void Close();
    void Flush();

private:
    BYTE *memory;
    size_t length;

    DWORD pos;
};

#endif // MEMORYSTREAM_H


