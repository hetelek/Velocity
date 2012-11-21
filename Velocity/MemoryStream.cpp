#include "MemoryStream.h"
#include <QMessageBox>

MemoryStream::MemoryStream(BYTE *data, size_t length) :
    memory(data), length(length)
{
    WORD temp = 1;
    littleEndian = (*(BYTE*)&temp) == 1;
}

void MemoryStream::setPosition(DWORD pos)
{
    this->pos = pos;
}

DWORD MemoryStream::getPosition()
{
    return pos;
}

void MemoryStream::reverseBytes(BYTE *bytes, DWORD len)
{
    BYTE temp;
    for (DWORD i = 0; i < (len / 2); i++)
    {
        temp = bytes[i];
        bytes[i] = bytes[len - (i + 1)];
        bytes[len - (i + 1)] = temp;
    }
}

BYTE MemoryStream::readByte()
{
    return memory[pos++];
}

void MemoryStream::readBytes(BYTE *out, size_t length)
{
    memcpy(out, memory + pos, length);
    pos += length;
}

WORD MemoryStream::readWord()
{
    WORD toReturn = *(WORD*)&memory[pos];
    pos += 2;

    if (littleEndian)
        reverseBytes((BYTE*)&toReturn, 2);

    return toReturn;
}

DWORD MemoryStream::readDword()
{
    DWORD toReturn = *(DWORD*)&memory[pos];
    pos += 4;

    if (littleEndian)
        reverseBytes((BYTE*)&toReturn, 4);

    return toReturn;
}

UINT64 MemoryStream::readUInt64()
{
    UINT64 toReturn = *(UINT64*)&memory[pos];
    pos += 8;

    if (littleEndian)
        reverseBytes((BYTE*)&toReturn, 8);

    return toReturn;
}

void MemoryStream::write(BYTE b)
{
    memory[pos++] = b;
}

void MemoryStream::write(BYTE *buffer, size_t length)
{
    memcpy(memory + pos, buffer, length);
    pos += length;
}

void MemoryStream::write(WORD w)
{
    if (littleEndian)
        reverseBytes((BYTE*)&w, 2);

    *(WORD*)(memory + pos) = w;
    pos += 2;
}

void MemoryStream::write(DWORD d)
{
    if (littleEndian)
        reverseBytes((BYTE*)&d, 4);

    *(DWORD*)(memory + pos) = d;
    pos += 4;
}

void MemoryStream::write(UINT64 u)
{
    if (littleEndian)
        reverseBytes((BYTE*)&u, 8);

    *(UINT64*)(memory + pos) = u;
    pos += 8;
}


