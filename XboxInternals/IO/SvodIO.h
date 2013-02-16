#ifndef SVODIO_H
#define SVODIO_H

#include "../Disc/gdfx.h"
#include "BaseIO.h"
#include "../Stfs/XContentHeader.h"
#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT SvodIO : public BaseIO
{
public:
    SvodIO(XContentHeader *metadata, GdfxFileEntry entry, MultiFileIO *io);

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    void SaveFile(string savePath, void(*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    void OverwriteFile(string inPath, void (*progress)(void*, DWORD, DWORD) = NULL, void *arg = NULL);

    void SetPosition(UINT64 address, std::ios_base::seek_dir dir = std::ios_base::beg);

    UINT64 GetPosition();

    void Flush();

    void Close();

private:
    void SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex);

    MultiFileIO *io;
    XContentHeader *metadata;
    GdfxFileEntry fileEntry;
    DWORD pos;
    DWORD offset;
};

#endif // SVODIO_H