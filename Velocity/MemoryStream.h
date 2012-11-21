#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include "winnames.h"
#include <string.h>

class MemoryStream
{
public:
    MemoryStream(BYTE *data, size_t length);

    void setPosition(DWORD pos);
    DWORD getPosition();

    BYTE readByte();
    void readBytes(BYTE *out, size_t length);
    WORD readWord();
    DWORD readDword();
    UINT64 readUInt64();

    void write(BYTE b);
    void write(BYTE *buffer, size_t length);
    void write(WORD w);
    void write(DWORD d);
    void write(UINT64 u);

private:
    BYTE *memory;
    size_t length;

    DWORD pos;
    bool littleEndian;

    void reverseBytes(BYTE *bytes, DWORD len);
};

#endif // MEMORYSTREAM_H
