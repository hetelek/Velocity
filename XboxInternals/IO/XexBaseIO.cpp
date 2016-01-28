#include "XexBaseIO.h"


XexBaseIO::XexBaseIO(BaseIO *io, Xbox360Executable *xex) :
    io(io), xex(xex), position(0)
{

}

void XexBaseIO::SetPosition(UINT64 position, ios_base::seek_dir dir)
{
    if (position != 0 || dir != std::ios_base::beg)
        throw std::string("XexBaseIO: Can only seek to the beginning.");

    xex->io->SetPosition(xex->header.dataAddress);
    this->position = 0;
}

UINT64 XexBaseIO::GetPosition()
{
    return position;
}

void XexBaseIO::Flush()
{
    xex->io->Flush();
}

void XexBaseIO::Close()
{
    // don't want to close the XEX's IO so don't do anything
}
