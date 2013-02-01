#include "MemoryIO.h"

MemoryIO::MemoryIO(BYTE *data, size_t length) :
    BaseIO(), memory(data), length(length)
{

}

void MemoryIO::SetPosition(DWORD pos)
{
    this->pos = pos;
}

DWORD MemoryIO::GetPosition()
{
    return pos;
}

void MemoryIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    memcpy(outBuffer, memory + pos, len);
    pos += len;
}

void MemoryIO::WriteBytes(BYTE *buffer, DWORD len)
{
    memcpy(memory + pos, buffer, len);
    pos += len;
}



