#include "XeCrypt.h"

void XeCrypt::BnQw_SwapDwQwLeBe(BYTE *data, DWORD length)
{
    if (length % 8 != 0)
        throw std::string("STFS: length is not divisible by 8.\n");

    for (DWORD i = 0; i < length / 2; i += 8)
    {
        BYTE temp[8];
        memcpy(temp, &data[i], 8);

        BYTE temp2[8];
        memcpy(temp2, &data[length - i - 8], 8);

        memcpy(&data[i], temp2, 8);
        memcpy(&data[length - i - 8], temp, 8);
    }
}
