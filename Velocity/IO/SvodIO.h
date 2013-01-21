#ifndef SVODIO_H
#define SVODIO_H

#include "../Disc/gdfx.h"
#include "BaseIO.h"
#include "../Stfs/XContentHeader.h"

class SvodIO : public BaseIO
{
public:
    SvodIO(XContentHeader *metadata, GDFXFileEntry entry, MultiFileIO *io);

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    void SaveFile(string savePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    void OverwriteFile(string inPath, void (*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    void SetPosition(DWORD address);

    void Close();

private:
    void SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex);

    MultiFileIO *io;
    XContentHeader *metadata;
    GDFXFileEntry fileEntry;
    DWORD pos;
    DWORD offset;
};

#endif // SVODIO_H
