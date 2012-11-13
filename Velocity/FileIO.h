#pragma once

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include "winnames.h"

using std::string;
using std::wstring;
using std::fstream;
using std::streampos;
using std::ios_base;

enum EndianType
{
	BigEndian,
	LittleEndian,
	Default
};

class FileIO
{
public:
	FileIO(string path, bool truncate = false);
    void setPosition(streampos pos, ios_base::seek_dir dir = ios_base::beg);
	streampos getPosition();
	void close();
	void flush();

	// reading ints
	INT16 readInt16();
	WORD readWord();
	INT24 readInt24(EndianType et = Default);
	DWORD readDword();
	INT32 readInt32();
	INT64 readInt64();
	UINT64 readUInt64();
	UINT64 readMultiByte(size_t size);

    float readFloat();
    double readDouble();

    // reading strings
	string readString(size_t len = 0);
	wstring readWString(size_t len = 0);

	// other reading
	void readBytes(BYTE *buffer, size_t size);
	BYTE readByte();


	// writing raw data
	void write(void *destination, size_t len);

	// writing ints
	void write(INT16 val);
	void write(WORD val);
	void write(INT24 val, EndianType et = Default);
	void write(DWORD val);
	void write(INT32 val);
	void write(INT64 val);
	void write(UINT64 val);
	void write(BYTE val);

	// writing strings
	void write(string val, int forceLen = -1);
	void write(wstring val);

	void swapEndian();

	string getFilePath();
	EndianType getEndian();

	void setEndian(EndianType byteOrder);

	static void swapEndian(void *arr, int elemSize, int len);
	~FileIO();
private:
	EndianType endian;
	void readBytesWithChecks(void *buffer, INT32 size);
	fstream *fstr;
	const string filePath;
};

