#ifndef XEXAESIO_H
#define XEXAESIO_H

#include "BaseIO.h"
#include "XexBaseIO.h"
#include "Xex/Xex.h"

#include <botan/botan.h>
#include <botan/sha160.h>
#include <botan/aes.h>

#define XEX_NULL_BUFFER_SIZE 	0x10000

class XexAesIO : public XexBaseIO
{
public:
    XexAesIO(BaseIO *io, Xbox360Executable *xex, const BYTE *key);
    ~XexAesIO();

    void ReadBytes(BYTE *outBuffer, DWORD readLength);

    void WriteBytes(BYTE *buffer, DWORD len);

    UINT64 Length();

private:
    BYTE curIV[XEX_AES_BLOCK_SIZE];
    BYTE curDecryptedBlock[XEX_AES_BLOCK_SIZE];
    UINT64 curAesBlockAddress;
    Botan::AES_128 *aes;

    void AesCbcDecrypt(const BYTE *bufferEnc);
};

#endif // XEXAESIO_H
