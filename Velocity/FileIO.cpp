#include "FileIO.h"

#include <QString>

FileIO::FileIO(string path, bool truncate) : filePath(path)
{
    fstr = new fstream(path.c_str(), (std::_Ios_Openmode)(fstream::in | fstream::out | fstream::binary | (fstream::trunc * truncate)));
    if (!fstr->is_open())
        throw QString("FileIO: Error opening the file. %1\n").arg(errno);
	endian = BigEndian;
}

void FileIO::setPosition(streampos pos, ios_base::seek_dir dir)
{
    fstr->seekp(pos, (std::_Ios_Seekdir)dir);
}

streampos FileIO::getPosition()
{
	return fstr->tellp();
}

void FileIO::close()
{
    fstr->close();
}

void FileIO::flush()
{
	fstr->flush();
}

void FileIO::swapEndian(void *arr, int elemSize, int len)
{
    char temp[elemSize];

	for (int i = 0; i < (len / 2); i++)
	{
		memcpy(temp, ((char*)arr) + (i * elemSize), elemSize);
		memcpy(((char*)arr) + (i * elemSize), ((char*)arr) + (((len - 1) * elemSize) - (i * elemSize)), elemSize);
		memcpy(((char*)arr) + (((len - 1) * elemSize) - (i * elemSize)), temp, elemSize);
	}
}

void FileIO::swapEndian()
{
	endian = (EndianType)!endian;
}

string FileIO::getFilePath()
{
	return filePath;
}

EndianType FileIO::getEndian()
{
	return this->endian;
}

UINT64 FileIO::readMultiByte(size_t size)
{
	switch (size)
	{
	case 1:
		return readByte();
	case 2:
		return readWord();
	case 4:
		return readDword();
	case 8:
		return readUInt64();
	default:
        throw QString("FileIO: Invalid multi-byte size.\n");
	}
}

void FileIO::setEndian(EndianType byteOrder)
{
	this->endian = byteOrder;
}

// reading functions below
void FileIO::readBytes(BYTE *buffer, size_t size)
{
	fstr->read((char*)buffer, size);
}

void FileIO::readBytesWithChecks(void *buffer, INT32 size)
{
	fstr->read((char*)buffer, size);
	if(endian == BigEndian)
		swapEndian(buffer, 1, size);
}

INT16 FileIO::readInt16()
{
	return (INT16)readWord();
}

WORD FileIO::readWord()
{
	WORD returnVal;
	readBytesWithChecks(&returnVal, 2);
	return returnVal;
}

INT24 FileIO::readInt24(EndianType et)
{
	EndianType orig = endian;

	if(et != Default)
		endian = et;

	INT24 returnVal = readDword();

	if(endian == BigEndian)
		returnVal = (returnVal & 0xFFFFFF00) >> 8;
	else
		returnVal = returnVal & 0x00FFFFFF;

	setPosition((DWORD)fstr->tellg() - 1);
	endian = orig;

	return returnVal;
}

DWORD FileIO::readDword()
{
	DWORD returnVal;
	readBytesWithChecks(&returnVal, 4);

    #if defined __unix | defined __APPLE__
        return returnVal & 0x00000000FFFFFFFF;
    #else
        return returnVal;
    #endif
}

INT32 FileIO::readInt32()
{
	return (INT32)readDword();
}

UINT64 FileIO::readUInt64()
{
	UINT64 returnVal;
	readBytesWithChecks(&returnVal, 8);
	return returnVal;
}

INT64 FileIO::readInt64()
{
	return (INT64)readUInt64();
}


float FileIO::readFloat()
{
    float returnVal;
    readBytesWithChecks(&returnVal, 4);

    return returnVal;
}

double FileIO::readDouble()
{
    double returnVal;
    readBytesWithChecks(&returnVal, 8);

    return returnVal;
}

BYTE FileIO::readByte()
{
	BYTE returnVal;
	readBytes(&returnVal, 1);
	return returnVal;
}

string FileIO::readString(size_t len)
{
	size_t pos = (size_t)fstr->tellg() + len;
	string returnVal = "";
	char readChar = (char)readByte();

	while (readChar != 0 && len-- != 1) 
	{
		returnVal += readChar;
		readChar = (char)readByte();
	}

	fstr->seekg(pos);
	return returnVal;
}

wstring FileIO::readWString(size_t len)
{
	size_t pos = (size_t)fstr->tellg();
	bool lenIn = (len != 0);
	wstring returnVal = L"";
	wchar_t readChar = (wchar_t)readWord();

	while (readChar != 0 && len-- != 1)
	{
		returnVal += readChar;
		readChar = (wchar_t)readWord();
	} 

	if (!lenIn)
		fstr->seekg(pos + ((returnVal.size() + 1) * 2));
	else
		fstr->seekg(pos + len);
	return returnVal;
}


// writing functions below
void FileIO::write(void *destination, size_t len)
{
	fstr->write((char*)destination, len);
}

void FileIO::write(WORD val)
{
	if(endian == BigEndian)
		swapEndian(&val, 1, 2);
	write(&val, 2);
}

void FileIO::write(INT16 val)
{
	if(endian == BigEndian)
		swapEndian(&val, 1, 2);
	write(&val, 2);
}

void FileIO::write(DWORD val)
{
	if(endian == BigEndian)
		swapEndian(&val, 1, 4);
	write(&val, 4);
}

void FileIO::write(INT32 val)
{
	if(endian == BigEndian)
		swapEndian(&val, 1, 4);
	write(&val, 4);
}

void FileIO::write(INT64 val)
{
	if(endian == BigEndian)
		swapEndian(&val, 1, 8);
	write(&val, 8);
}

void FileIO::write(UINT64 val)
{
	if(endian == BigEndian)
		swapEndian(&val, 1, 8);
	write(&val, 8);
}

void FileIO::write(INT24 val, EndianType et)
{	
	EndianType orig = endian;
	if(et != Default)
		endian = et;

	if(endian == BigEndian)
	{
		val <<= 8;
		swapEndian(&val, 1, 4);
	}
	write(&val, 3);

	endian = orig;
}

void FileIO::write(string val, int forceLen)
{
	write((void*)val.c_str(), val.size());

	if(forceLen > 0)
	{
		forceLen -= val.size();

		for (int i = 0; i < forceLen; i++)
			write((BYTE)0);
	}
}

void FileIO::write(wstring val)
{
	if (endian == LittleEndian)
		write((void*)val.c_str(), (val.size() + 1) * 2);
	else
	{
		for (DWORD i = 0; i < val.size(); i++)
			write(*(WORD*)&val.at(i));
		write((WORD)0);
	}
}

void FileIO::write(BYTE val)
{
	write(&val, 1);
}

FileIO::~FileIO(void)
{
	if(fstr->is_open())
        fstr->close();
    delete fstr;
}
