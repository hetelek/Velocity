#include "YTGR.h"

YTGR::YTGR(std::string filePath) : ioPassedIn(false)
{
    io = new FileIO(filePath);
    Parse();
}

YTGR::YTGR(FileIO *io) : ioPassedIn(true)
{
    this->io = io;
    Parse();
}

void YTGR::Parse()
{
    io->setPosition(0);

    // verify the magic
    magic = io->readDword();
    if (magic != 0x59544752)
        throw std::string("YTGR: Invalid magic.");

    // read the rest of the header
    io->setEndian(LittleEndian);
    XSignerMinimumVersion = io->readDword();
    structSize = io->readDword();
    if (structSize != 0x130)
        throw std::string("YTGR: Invalid struct size.");

    // parse the timestamp
    WINFILETIME fileTime;
    fileTime.dwLowDateTime = io->readDword();
    fileTime.dwHighDateTime = io->readDword();
    dateAddedToServer = XDBFHelpers::FILETIMEtoTimeT(fileTime);

    contentLength = io->readDword();
    reserved = io->readDword();
    io->readBytes(contentHash, 0x14);
    io->readBytes(rsaSignature, 0x100);

    // verify the signature
    BYTE message[0x30];
    io->setPosition(0);
    io->readBytes(message, 0x30);
    valid = XeKeys::VerifyRSASignature(LIVEKey, message, 0x30, rsaSignature) || XeKeys::VerifyRSASignature(UnknownKey, message, 0x30, rsaSignature);
}
