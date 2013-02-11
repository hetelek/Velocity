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

YTGR::~YTGR()
{
    if (!ioPassedIn)
        delete io;
}

void YTGR::Parse()
{
    io->setPosition(0);

    // verify the magic
    magic = io->readDword();
    if (magic != 0x59544752)
        throw std::string("YTGR: Invalid magic.");

    // read the version
    io->setEndian(LittleEndian);
    DWORD tempV = io->readDword();
    XSignerMinimumVersion.major = tempV >> 28;
    XSignerMinimumVersion.minor = (tempV >> 24) & 0xF;
    XSignerMinimumVersion.build = (tempV >> 8) & 0xFFFF;
    XSignerMinimumVersion.revision = tempV & 0xFF;

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

    // verify the hash
    BYTE block[0x1000];
    BYTE calculatedHash[0x14];
    Botan::SHA_160 sha1;
    sha1.clear();

    io->setPosition(0x130);
    DWORD tempLen = contentLength;

    while (tempLen >= 0x1000)
    {
        io->readBytes(block, 0x1000);
        sha1.update(block, 0x1000);
        tempLen -= 0x1000;
    }
    if (tempLen != 0)
    {
        io->readBytes(block, tempLen);
        sha1.update(block, tempLen);
    }

    sha1.final(calculatedHash);

    // make sure both the signature and hash are valid
    valid = valid && !memcmp(contentHash, calculatedHash, 0x14);
}
