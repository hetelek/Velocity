#pragma once

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <XboxInternals/TypeDefinitions.h>
#include <XboxInternals/IO/BaseIO.h>

using std::string;
using std::wstring;
using std::fstream;
using std::streampos;
using std::ios_base;

class XBOXINTERNALS_EXPORT FileIO : public BaseIO
{
public:
    FileIO(string path, bool truncate = false);
    void SetPosition(UINT64 pos, ios_base::seekdir dir = ios_base::beg);
    UINT64 GetPosition();
    UINT64 Length();

    void ReadBytes(BYTE *outBuffer, DWORD len);
    void WriteBytes(BYTE *buffer, DWORD len);

    void Close();
    void Flush();

    string GetFilePath();

    static void ReverseGenericArray(void *arr, int elemSize, int len);
    virtual ~FileIO();
private:
    EndianType endian;
    UINT64 length;
    void ReadBytesWithChecks(void *buffer, INT32 size);
    fstream *fstr;
    const string filePath;
};



