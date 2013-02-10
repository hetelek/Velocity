#include "XeKeys.h"

bool XeKeys::VerifyRSASignature(XeKeysRsaKeys key, BYTE *hash, BYTE *signature)
{
    BYTE *n;
    DWORD exponent;

    // get the correct key
    switch (key)
    {
        case PIRSKey:
            n = Pirs1N;
            exponent = 3;
            break;
        case LIVEKey:
            n = Live1N;
            exponent = 0x10001;
            break;
        case DeviceKey:
            n = Device1N;
            exponent = 3;
            break;
        case UnknownKey:
            n = Unknown1N;
            exponent = 3;
            break;
        default:
            throw std::string("XeKeys: Invalid key.\n");
    }

    XeCrypt::BnQw_SwapDwQwLeBe(n, 0x80);
    Botan::RSA_PublicKey pubKey(Botan::BigInt::decode(n, 0x80), exponent);

    Botan::PK_Verifier verifier(pubKey, "EMSA3(SHA-160)");

    return verifier.verify_message(hash, 0x14, signature, 0x100);
}
