#include "IO/FileIO.h"
#include <vector>

FileIO::FileIO(string path, bool truncate) :
    BaseIO(), filePath(path)
{
    fstr = new fstream(path.c_str(), fstream::in | fstream::out | fstream::binary | (truncate ? fstream::trunc : static_cast<std::ios_base::openmode>(0)));
    if (!fstr->is_open())
    {
        std::string ex("FileIO: Error opening the file. ");
        ex += strerror(errno);
        ex += "\n";
        throw ex;
    }

	endian = BigEndian;

    fstr->rdbuf()->pubsetbuf(0, 0);
    fstr->seekp(0);
}

void FileIO::SetPosition(UINT64 pos, ios_base::seek_dir dir)
{
    fstr->seekp(pos, dir);
}

UINT64 FileIO::GetPosition()
{
    return fstr->tellp();
}

UINT64 FileIO::Length()
{
    UINT64 originalPosition = GetPosition();

    fstr->seekp(0, std::ios_base::end);
    UINT64 fileLength = fstr->tellp();

    SetPosition(originalPosition);
    return fileLength;
}

void FileIO::Close()
{
    fstr->close();
}

void FileIO::Flush()
{
    fstr->flush();
}

void FileIO::Resize(UINT64 size)
{
    if (size > this->Length())
        throw std::string("FileIO: Cannot expand file size.");

    BYTE *buffer = new BYTE[0x10000];
    std::string newFilePath = filePath + ".new";

    // open a new stream
    fstream *newFileStream = new fstream(newFilePath.c_str(), fstream::out | fstream::binary | fstream::trunc);
    if (!fstr->is_open())
    {
        std::string ex("FileIO: Failed to resize file. ");
        ex += strerror(errno);
        ex += "\n";
        throw ex;
    }

    // copy the data
    while (size >= 0x10000)
    {
        fstr->read((fstream::char_type*)buffer, 0x10000);
        newFileStream->write((fstream::char_type*)buffer, 0x10000);
        size -= 0x10000;
    }

    if (size != 0)
    {
        fstr->read((fstream::char_type*)buffer, size);
        newFileStream->write((fstream::char_type*)buffer, size);
    }
    delete buffer;

    // close the current stream, delete the files
    fstr->close();
    remove(filePath.c_str());

    newFileStream->close();
    delete newFileStream;

    // rename the new file to the correct original name
    if (rename(newFilePath.c_str(), filePath.c_str()) != 0)
        throw std::string("FileIO: Failed to resize file.");

    // set the final stream
    fstr = new fstream(filePath.c_str(), fstream::in | fstream::out | fstream::binary);
}

void FileIO::ReverseGenericArray(void *arr, int elemSize, int len)
{
	std::vector<char> tempVec;
	tempVec.reserve(elemSize);
	char* temp = tempVec.data();

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
    fstr->read((fstream::char_type*)outBuffer, len);
    if (fstr->fail())
        throw string("FileIO: Error reading from file.\n") + string(strerror(errno));
}

void FileIO::WriteBytes(BYTE *buffer, DWORD len)
{
    fstr->write((fstream::char_type*)buffer, len);
    if (fstr->fail())
        throw string("FileIO: Error writing to file.\n") + string(strerror(errno));
}

FileIO::~FileIO(void)
{
	if(fstr->is_open())
        fstr->close();
    delete fstr;
}
