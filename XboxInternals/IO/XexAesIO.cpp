#include "XexAesIO.h"


XexAesIO::XexAesIO(BaseIO *io, Xbox360Executable *xex, const BYTE *key) :
    XexBaseIO(io, xex), curAesBlockAddress(0)
{
    // the initialization vector starts out as all zeros
    memset(curIV, 0, XEX_AES_BLOCK_SIZE);

    // create the aes object
    aes = new Botan::AES_128();
    aes->set_key(key, XEX_AES_BLOCK_SIZE);

    // decrypt the first block in the file
    BYTE buffer[XEX_AES_BLOCK_SIZE];
    xex->io->SetPosition(xex->header.dataAddress);
    xex->io->ReadBytes(buffer, XEX_AES_BLOCK_SIZE);
    AesCbcDecrypt(buffer);
}

XexAesIO::~XexAesIO()
{
    delete aes;
}

void XexAesIO::ReadBytes(BYTE *outBuffer, DWORD readLength)
{
    DWORD bytesRead = 0;
    DWORD bytesLeft = readLength;

    // check to see if we already have decrypted the data the caller requested
    if (position <= curAesBlockAddress + XEX_AES_BLOCK_SIZE)
    {
        // calculate the number of bytes to read
        DWORD bytesToRead = (curAesBlockAddress + XEX_AES_BLOCK_SIZE) - position;

        // calculate the offset into the decrypted block to start 'reading' from
        DWORD offset = position - curAesBlockAddress;

        // read them into the buffer
        memcpy(outBuffer, curDecryptedBlock + offset, bytesToRead);
        bytesRead += bytesToRead;
        position += bytesToRead;
        bytesLeft -= bytesToRead;
    }

    // as long as there are bytes left to read, read them in
    BYTE encryptedBuffer[XEX_AES_BLOCK_SIZE];
    while (bytesLeft != 0)
    {
        // seek to the beginning of the next AES block to read
        UINT64 nextAesBlockAddress = xex->header.dataAddress + curAesBlockAddress + XEX_AES_BLOCK_SIZE;
        xex->io->SetPosition(nextAesBlockAddress);

        xex->io->ReadBytes(encryptedBuffer, XEX_AES_BLOCK_SIZE);
        AesCbcDecrypt(encryptedBuffer);
        curAesBlockAddress += XEX_AES_BLOCK_SIZE;

        // calculate the amount of bytes to copy over
        DWORD bytesToCopy = XEX_AES_BLOCK_SIZE;
        if (bytesLeft < XEX_AES_BLOCK_SIZE)
            bytesToCopy = bytesLeft;

        memcpy(outBuffer + bytesRead, curDecryptedBlock, bytesToCopy);

        bytesRead += bytesToCopy;
        position += bytesToCopy;
        bytesLeft -= bytesToCopy;
    }

    // check if we need to decrypt the next block, make sure we're not at the end of the file
    if (position != Length() && (position & ~0xF) == position)
    {
        xex->io->ReadBytes(encryptedBuffer, XEX_AES_BLOCK_SIZE);
        AesCbcDecrypt(encryptedBuffer);
        curAesBlockAddress += XEX_AES_BLOCK_SIZE;
    }
}

void XexAesIO::WriteBytes(BYTE *buffer, DWORD len)
{

}

UINT64 XexAesIO::Length()
{
    return xex->io->Length() - xex->header.dataAddress;
}

void XexAesIO::AesCbcDecrypt(const BYTE *bufferEnc)
{
    aes->decrypt(bufferEnc, curDecryptedBlock);

    for (DWORD x = 0; x < XEX_AES_BLOCK_SIZE; x++)
    {
        // xor the data with the old IV
        curDecryptedBlock[x] ^= curIV[x];

        // update the IV
        curIV[x] = bufferEnc[x];
    }
}
