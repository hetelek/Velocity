#pragma once

#include <iostream>
#include "winnames.h"

using std::string;
using std::wstring;

enum EndianType
{
    BigEndian,
    LittleEndian,
    Default
};

class BaseIO
{
public:
    BaseIO();

    // set the byte order in which to read the bytes
    void SetEndian(EndianType byteOrder);

    // get the byte order in which to read the bytes
    EndianType GetEndian();

    // read len bytes from the current file at the current position into buffer
    virtual void ReadBytes(BYTE *outBuffer, DWORD len) = 0;

    // write len bytes from the current file at the current position into buffer
    virtual void WriteBytes(BYTE *buffer, DWORD len) = 0;

    // all the read functions
    BYTE ReadByte();
    WORD ReadWord();
    DWORD ReadDword();
    UINT64 ReadUint64();
    string ReadString(int len = -1);
    wstring ReadWString(int len = -1);

    // write functions
    void Write(BYTE b);
    void Write(WORD w);
    void Write(DWORD dw);
    void Write(UINT64 u64);
    void Write(string s);
    void Write(wstring ws);

protected:
    EndianType byteOrder;

private:
    void reverseByteArray(BYTE *array, DWORD len);
};
