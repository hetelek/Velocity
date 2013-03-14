/* Most parts of this class were originally developed by Lander Griffith (https://github.com/landr0id/).
   Much of his code is used throughout this class or very slightly modified */

#ifndef MULTIFILEIO_H
#define MULTIFILEIO_H

#include <vector>

#include "FileIO.h"

class XBOXINTERNALSSHARED_EXPORT MultiFileIO : public BaseIO
{
public:
    MultiFileIO(std::vector<std::string> filePaths);
    MultiFileIO(std::vector<BaseIO*> files);

    // seek to a position in a file
    void SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg);

    // get current address in the file
    UINT64 GetPosition();

    // get the length of the current file
    UINT64 Length();

    // get the byte order in which to read the bytes
    EndianType GetEndian();

    // read len bytes from the current file at the current position into buffer
    void ReadBytes(BYTE *outBuffer, DWORD len);

    // write len bytes from the current file at the current position into buffer
    void WriteBytes(BYTE *buffer, DWORD len);

private:
    UINT64 pos;
    DWORD currentIOIndex;
    std::vector<BaseIO*> files;
};

#endif // MULTIFILEIO_H
