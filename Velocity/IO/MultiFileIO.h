#ifndef MULTIFILEIO_H
#define MULTIFILEIO_H

#include "FileIO.h"
#include <iostream>
#include <vector>
#include "dirent.h"
#include <QDebug>
#include "BaseIO.h"

using std::string;
using std::wstring;
using std::vector;

class MultiFileIO : public BaseIO
{
public:
    MultiFileIO(string fileDirectory);
    ~MultiFileIO();

    // seek to a certain address in the file, index of -1 for current file
    void SetPosition(DWORD addressInFile, int fileIndex = -1);

    // get the current position of the io
    void GetPosition(DWORD *addressInFile, int *fileIndex);

    // read len bytes from the current file at the current position into buffer
    void ReadBytes(BYTE *outBuffer, DWORD len);

    // write len bytes to the file at the current position
    void WriteBytes(BYTE *buffer, DWORD len);

    void Close();

    // get the number of files in the directory
    DWORD FileCount();

    // get the number of bytes in the current file
    DWORD CurrentFileLength();

private:
    DWORD addressInFile;
    DWORD fileIndex;

    FileIO *currentIO;
    vector<string> files;

    // get all the file names in the directory
    void loadDirectories(string path);
};

#endif // MULTIFILEIO_H
