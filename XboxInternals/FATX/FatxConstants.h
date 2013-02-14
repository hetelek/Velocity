#ifndef FATXCONSTANTS_H
#define FATXCONSTANTS_H

#include "../winnames.h"

#define FAT32 4
#define FAT16 2

#define FATX_MAGIC 0x58544146
#define FAT_CLUSTER16_RESERVED (WORD)0xfff0

struct HddSizes
{
    static const INT64 SystemCache = 0x80000000;
    static const INT64 GameCache = 0xA0E30000;
    static const INT64 SystemPartition = 0x10000000;
    static const INT64 SystemAux = 0xCE30000;
    static const INT64 SystemExtended = 0x8000000;
};

struct HddOffsets
{
    static const INT64 Data = 0x130EB0000;
    static const INT64 Josh = 0x800;
    static const INT64 SecuritySector = 0x2000;
    static const INT64 SystemCache = 0x80000;
    static const INT64 GameCache = 0x80080000;
    static const INT64 SystemAux = 0x10C080000;
    static const INT64 SystemExtended = 0x118EB0000;
    static const INT64 SystemPartition = 0x120EB0000;
};

#endif // FATXCONSTANTS_H
