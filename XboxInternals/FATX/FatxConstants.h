#ifndef FATXCONSTANTS_H
#define FATXCONSTANTS_H

#include "../winnames.h"

#define FAT32 4
#define FAT16 2

#define FATX_MAGIC 0x58544146

#define FATX_ENTRY_SIZE 0x40
#define FATX_ENTRY_MAX_NAME_LENGTH 0x2A
#define FATX_ENTRY_DELETED 0xE5

#define FAT_CLUSTER_AVAILABLE (DWORD)0x00000000
#define FAT_CLUSTER_RESERVED (DWORD)0xfffffff0
#define FAT_CLUSTER_LAST (DWORD)0xffffffff

#define FAT_CLUSTER16_AVAILABLE (WORD)0x0000
#define FAT_CLUSTER16_RESERVED (WORD)0xfff0
#define FAT_CLUSTER16_LAST (WORD)0xffff

enum FatxDirentAttributes
{
    FatxReadOnly = 0x01,
    FatxHidden = 0x02,
    FatxSystem = 0x04,
    FatxDirectory = 0x10,
    FatxArchive = 0x20,
    FatxDevice = 0x40,
    FatxNormal = 0x80
};

struct HddSizes
{
    static const INT64 SystemCache = 0x80000000;
    static const INT64 GameCache = 0xA0E30000;
    static const INT64 SystemPartition = 0x10000000;
    static const INT64 SystemAuxiliary = 0xCE30000;
    static const INT64 SystemExtended = 0x8000000;
};

struct HddOffsets
{
    static const INT64 Data = 0x130EB0000;
    static const INT64 Josh = 0x800;
    static const INT64 SecuritySector = 0x2000;
    static const INT64 SystemCache = 0x80000;
    static const INT64 GameCache = 0x80080000;
    static const INT64 SystemAuxiliary = 0x10C080000;
    static const INT64 SystemExtended = 0x118EB0000;
    static const INT64 SystemPartition = 0x120EB0000;
};

#endif // FATXCONSTANTS_H
