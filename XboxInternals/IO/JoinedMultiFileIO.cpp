#include "JoinedMultiFileIO.h"

JoinedMultiFileIO::JoinedMultiFileIO(std::vector<std::string> filePaths) : currentIOIndex(0), pos(0)
{
    for (int i = 0; i < filePaths.size(); i++)
    {
        FileIO *io = new FileIO(filePaths.at(i));
        files.push_back(io);
    }

    calcualteLengthOfAllFiles();
}

JoinedMultiFileIO::JoinedMultiFileIO(std::vector<BaseIO*> files) : files(files), currentIOIndex(0), pos(0)
{
    calcualteLengthOfAllFiles();
}

JoinedMultiFileIO::~JoinedMultiFileIO()
{
    Close();
}

void JoinedMultiFileIO::SetPosition(UINT64 position, std::ios_base::seek_dir dir)
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

UINT64 JoinedMultiFileIO::GetPosition()
{
    return pos;
}

UINT64 JoinedMultiFileIO::Length()
{
    return lengthOfFiles;
}

void JoinedMultiFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
    if (len > lengthOfFiles - pos)
        throw std::string("MultiFileIO: Requested Write length is too large.\n");

    DWORD offset = 0;

    // check to see if we can Write straight from the current stream
    if (files.at(currentIOIndex)->GetPosition() + len > files.at(currentIOIndex)->Length())
    {
        // if we can't, we must calculate how much we should read each time
        while (len > 0)
        {
            DWORD WriteCount = (files.at(currentIOIndex)->Length() > files.at(currentIOIndex)->GetPosition() + len) ? len : (files.at(currentIOIndex)->Length() - files.at(currentIOIndex)->GetPosition());
            files.at(currentIOIndex)->WriteBytes(buffer + offset, WriteCount);
            offset += WriteCount;
            len -= WriteCount;
            SetPosition(pos + WriteCount);
        }
    }
    else
    {
        files.at(currentIOIndex)->WriteBytes(buffer, len);
        pos += len;
    }
}

void JoinedMultiFileIO::Flush()
{
    files.at(currentIOIndex)->Flush();
}

void JoinedMultiFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    DWORD offset = 0;

    // check to see if we can read straight from the current stream
    if (files.at(currentIOIndex)->GetPosition() + len > files.at(currentIOIndex)->Length())
    {
        // if we can't, we must calculate how much we should read each time
        while (len > 0)
        {
            DWORD readCount = (files.at(currentIOIndex)->Length() > files.at(currentIOIndex)->GetPosition() + len) ? len : (files.at(currentIOIndex)->Length() - files.at(currentIOIndex)->GetPosition());
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

void JoinedMultiFileIO::Close()
{
    if (isClosed)
        return;

    for (int i = 0; i < files.size(); i++)
        delete files.at(i);

    files.clear();

    isClosed = true;
}

void JoinedMultiFileIO::calcualteLengthOfAllFiles()
{
    isClosed = false;
    lengthOfFiles = 0;

    for (int i = 0; i < files.size(); i++)
        lengthOfFiles += files.at(i)->Length();
}
