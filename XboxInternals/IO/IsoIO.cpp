#include "IsoIO.h"

IsoIO::IsoIO(BaseIO *isoIO, GdfxFileEntry *entry, ISO *iso) :
    isoIO(isoIO), entry(entry), iso(iso)
{
    SetPosition(0);
}

IsoIO::IsoIO(BaseIO *isoIO, std::string isoFilePath, ISO *iso) :
    isoIO(isoIO), iso(iso)
{
    entry = iso->GetFileEntry(isoFilePath);
    SetPosition(0);
}

void IsoIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    SetPosition(GetPosition());

    // check for end of file
    if (GetPosition() + len > Length())
        throw std::string("IsoIO: Cannot read beyond end of file.");

    isoIO->ReadBytes(outBuffer, len);
    virtualPosition += len;
}

void IsoIO::WriteBytes(BYTE *buffer, DWORD len)
{
    SetPosition(GetPosition());

    // check for end of file
    if (GetPosition() + len > Length())
        throw std::string("IsoIO: Cannot read beyond end of file.");

    isoIO->WriteBytes(buffer, len);
    virtualPosition += len;
}

UINT64 IsoIO::GetPosition()
{
    return virtualPosition;
}

void IsoIO::SetPosition(UINT64 position, std::ios_base::seek_dir dir)
{
    UINT64 newVirtualPosition = position;
    switch (dir)
    {
        case std::ios_base::beg:
            break;
        case std::ios_base::cur:
            newVirtualPosition += GetPosition();
            break;
        case std::ios_base::end:
            newVirtualPosition += Length();
            break;
        default:
            throw std::string("IsoIO: Unsupported seek direction");
    }

    if (newVirtualPosition > Length())
        throw std::string("IsoIO: Cannot seek beyond end of file.");

    virtualPosition = newVirtualPosition;

    // calculate the real position
    UINT64 realPosition = iso->SectorToAddress(entry->sector) + newVirtualPosition;
    isoIO->SetPosition(realPosition);
}

UINT64 IsoIO::Length()
{
    return entry->size;
}

void IsoIO::Flush()
{
    isoIO->Flush();
}

void IsoIO::Close()
{

}
