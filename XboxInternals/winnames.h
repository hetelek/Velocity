#ifndef WINNAMES_H
#define WINNAMES_H

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int INT32;
typedef long long INT64;
typedef unsigned long long UINT64;

typedef struct _WINFILETIME {
    DWORD dwHighDateTime;
    DWORD dwLowDateTime;
} WINFILETIME;

typedef short INT16;
typedef signed int INT24;

#endif // WINNAMES_H