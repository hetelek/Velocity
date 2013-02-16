#include "IO/FileIO.h"

FileIO::FileIO(string path, bool truncate) :
    BaseIO(), filePath(path)
{
    fstr = new fstream(path.c_str(), (std::_Ios_Openmode)(fstream::in | fstream::out | fstream::binary | (fstream::trunc * truncate)));
    if (!fstr->is_open())
        throw std::string("FileIO: Error opening the file. " + string(strerror(errno)) + "\n");
	endian = BigEndian;
}

void FileIO::SetPosition(UINT64 pos, ios_base::seek_dir dir)
{
    fstr->seekp(pos, (std::_Ios_Seekdir)dir);
}

UINT64 FileIO::GetPosition()
{
	return fstr->tellp();
}

void FileIO::Close()
{
    fstr->close();
}

void FileIO::Flush()
{
	fstr->flush();
}

void FileIO::ReverseGenericArray(void *arr, int elemSize, int len)
{
    char temp[elemSize];

	for (int i = 0; i < (len / 2); i++)
	{
		memcpy(temp, ((char*)arr) + (i * elemSize), elemSize);
		memcpy(((char*)arr) + (i * elemSize), ((char*)arr) + (((len - 1) * elemSize) - (i * elemSize)), elemSize);
		memcpy(((char*)arr) + (((len - 1) * elemSize) - (i * elemSize)), temp, elemSize);
	}
}

string FileIO::GetFilePath()
{
	return filePath;
}

void FileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    fstr->read((BYTE*)outBuffer, len);
}

void FileIO::WriteBytes(BYTE *buffer, DWORD len)
{
    fstr->write((BYTE*)buffer, len);
}

FileIO::~FileIO(void)
{
	if(fstr->is_open())
        fstr->close();
    delete fstr;
}
