#ifndef XECRYPT_H
#define XECRYPT_H

#include "TypeDefinitions.h"
#include <iostream>
#include <string.h>

#include <botan_all.h>

#include "XboxInternals_global.h"

class XBOXINTERNALSSHARED_EXPORT XeCrypt
{
public:
    static void BnQw_SwapDwQwLeBe(BYTE *data, DWORD length);

    static bool Pkcs1Verify(BYTE *pbMessage, DWORD cbMessage, BYTE *pbSignature, DWORD cbSignature,
            UINT64 publicExponent, BYTE *modulus);

    // copied from wikibooks.org
    template<typename iter>
    static void InsertionSort(iter first, iter last)
    {
        iter min = first;
        for(iter i = first + 1; i < last; ++i)
            if (*i < *min)
                min = i;

        std::iter_swap(first, min);
        while(++first < last)
            for(iter j = first; *j < *(j - 1); --j)
                std::iter_swap( (j - 1), j );
    }
};

#endif // XECRYPT_H


