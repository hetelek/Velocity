#ifndef ISOIO_H
#define ISOIO_H

#include <string>

#include "Disc/ISO.h"
#include "Disc/Gdfx.h"

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT IsoIO : public BaseIO
{
public:
    IsoIO(BaseIO *isoIO, GdfxFileEntry *entry, ISO *iso);
    IsoIO(BaseIO *isoIO, std::string isoFilePath, ISO *iso);

    void ReadBytes(BYTE *outBuffer, DWORD len);

    void WriteBytes(BYTE *buffer, DWORD len);

    UINT64 GetPosition();

    void SetPosition(UINT64 position, std::ios_base::seek_dir dir = std::ios_base::beg);

    UINT64 Length();

    void Flush();

    void Close();

private:
    BaseIO *isoIO;
    GdfxFileEntry *entry;
    ISO *iso;

    UINT64 virtualPosition;
};

#endif // ISOIO_H
