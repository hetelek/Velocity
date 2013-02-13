#include "BaseIO.h"

using namespace std;

BaseIO::BaseIO() :
    byteOrder(BigEndian)
{
    // should be implemented by derived class
}

void BaseIO::SetEndian(EndianType byteOrder)
{
    this->byteOrder = byteOrder;
}

EndianType BaseIO::GetEndian()
{
    return this->byteOrder;
}

void BaseIO::reverseByteArray(BYTE *array, DWORD len)
{
    BYTE temp;
    for (DWORD i = 0; i < len / 2; i++)
    {
        temp = array[i];
        array[i] = array[len - i - 1];
        array[len - i - 1] = temp;
    }
}

BYTE BaseIO::ReadByte()
{
    BYTE toReturn;
    ReadBytes(&toReturn, 1);
    return toReturn;
}

WORD BaseIO::ReadWord()
{
    WORD toReturn;
    ReadBytes(reinterpret_cast<BYTE*>(&toReturn), 2);

    if (byteOrder == BigEndian)
        reverseByteArray(reinterpret_cast<BYTE*>(&toReturn), 2);

    return toReturn;
}

DWORD BaseIO::ReadDword()
{
    DWORD toReturn;
    ReadBytes(reinterpret_cast<BYTE*>(&toReturn), 4);

    if (byteOrder == BigEndian)
        reverseByteArray(reinterpret_cast<BYTE*>(&toReturn), 4);

    return toReturn;
}

UINT64 BaseIO::ReadUint64()
{
    UINT64 toReturn;
    ReadBytes(reinterpret_cast<BYTE*>(&toReturn), 8);

    if (byteOrder == BigEndian)
        reverseByteArray(reinterpret_cast<BYTE*>(&toReturn), 8);

    return toReturn;
}

string BaseIO::ReadString(int len)
{
    string toReturn;

    // assume it's null terminating
    if (len == -1)
    {
        toReturn = "";
        char nextChar;
        while ((nextChar = ReadByte()) != 0)
            toReturn.append(nextChar);
    }
    else
    {
        char str[len + 1];
        str[len] = 0;

        ReadBytes((BYTE*)str, len);
        toReturn = string(str);
    }

    return toReturn;
}

wstring BaseIO::ReadWString(int len)
{
    wstring toReturn;

    // assume it's null terminating
    if (len == -1)
    {
        toReturn = L"";
        wchar_t nextChar;
        while ((nextChar = ReadWord()) != 0)
            toReturn.append(&nextChar);
    }
    else
    {
        wchar_t str[len + 1];
        str[len] = 0;

        ReadBytes((BYTE*)str, (len + 1) * 2);

        // swap the byte order if needed
        if (byteOrder == BigEndian)
        {
            BYTE temp;
            BYTE *rawString = (BYTE*)str;
            for (DWORD i = 0; i < len; i++)
            {
                temp = rawString[i * 2];
                rawString[i * 2] = rawString[i * 2 + 1];
                rawString[i * 2 + 1] = temp;
            }
        }

        toReturn = wstring(str);
    }

    return toReturn;
}

void BaseIO::Write(BYTE b)
{
    WriteBytes(&b, 1);
}

void BaseIO::Write(WORD w)
{
    if (byteOrder == BigEndian)
        reverseByteArray(reinterpret_cast<BYTE*>(&w), 2);
    WriteBytes(reinterpret_cast<BYTE*>(&w), 2);
}

void BaseIO::Write(DWORD dw)
{
    if (byteOrder == BigEndian)
        reverseByteArray(reinterpret_cast<BYTE*>(&dw), 4);
    WriteBytes(reinterpret_cast<BYTE*>(&dw), 4);
}

void BaseIO::Write(UINT64 u64)
{
    if (byteOrder == BigEndian)
        reverseByteArray(reinterpret_cast<BYTE*>(&u64), 8);
    WriteBytes(reinterpret_cast<BYTE*>(&u64), 8);
}

void BaseIO::Write(string s, bool nullTerminating)
{
    WriteBytes(s.c_str(), s.length() + nullTerminating);
}

void BaseIO::Write(wstring ws, bool nullTerminating)
{
    if (byteOrder == LittleEndian)
        WriteBytes((BYTE*)ws.c_str(), (ws.length() + nullTerminating) * 2);
    else
    {
        WORD curChar;
        for (DWORD i = 0; i < ws.length() + nullTerminating; i++)
        {
            curChar = ws.at(i);
            reverseByteArray(reinterpret_cast<BYTE*>(&curChar), 2);
            WriteBytes(reinterpret_cast<BYTE*>(curChar), 2);
        }
    }
}
