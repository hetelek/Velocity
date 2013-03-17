#include "MultiFileIO.h"

MultiFileIO::MultiFileIO(std::vector<std::string> filePaths) : currentIOIndex(0), pos(0)
{
    for (int i = 0; i < filePaths.size(); i++)
    {
        FileIO *io = new FileIO(filePaths.at(i));
        files.push_back(io);
    }

    calcualteLengthOfAllFiles();
}

MultiFileIO::MultiFileIO(std::vector<BaseIO*> files) : files(files), currentIOIndex(0), pos(0)
{
    calcualteLengthOfAllFiles();
}

void MultiFileIO::SetPosition(UINT64 position, std::ios_base::seek_dir dir)
{
    if (dir == std::ios_base::end)
        throw std::string("MultiFileIO: Unsupported seek dir.");
    else if (dir == std::ios_base::cur)
        position += pos;

    pos = position;

    // calculate which stream we should use
    for (int i = 0; i < files.size(); i++)
    {
        if (position >= files.at(i)->Length())
            position -= files.at(i)->Length();
        else
        {
            currentIOIndex = i;
            files.at(i)->SetPosition(position);
            break;
        }
    }
}

UINT64 MultiFileIO::GetPosition()
{
    return pos;
}

UINT64 MultiFileIO::Length()
{
    return lengthOfFiles;
}

void MultiFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
}

void MultiFileIO::Flush()
{
    files.at(currentIOIndex)->Flush();
}

void MultiFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    DWORD offset = 0;

    // check to see if we can read straight from the current stream
    if (files.at(currentIOIndex)->GetPosition() + len > files.at(currentIOIndex)->Length())
    {
        // if we can't, we must calculate how much we should read each time
        while (len > 0)
        {
            DWORD readCount = (files.at(currentIOIndex)->Length() > files.at(currentIOIndex)->GetPosition() + len) ? len : files.at(currentIOIndex)->Length();
            files.at(currentIOIndex)->ReadBytes(outBuffer + offset, readCount);
            offset += readCount;
            len -= readCount;
            SetPosition(pos + readCount);
        }
    }
    else
    {
        files.at(currentIOIndex)->ReadBytes(outBuffer, len);
        pos += len;
    }
}

void MultiFileIO::Close()
{
    for (int i = 0; i < files.size(); i++)
    {
        files.at(i)->Close();
        delete files.at(i);
        files.erase(files.begin() + i);
    }
}

void MultiFileIO::calcualteLengthOfAllFiles()
{
    lengthOfFiles = 0;

    for (int i = 0; i < files.size(); i++)
        lengthOfFiles += files.at(i)->Length();
}
