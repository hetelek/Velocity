#ifndef WINNAMES_H
#define WINNAMES_H

typedef unsigned int INT24;
typedef unsigned char BYTE;
typedef short INT16;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int INT32;
typedef long long INT64;
typedef unsigned long long UINT64;

typedef struct _WINFILETIME
{
    DWORD dwHighDateTime;
    DWORD dwLowDateTime;

} WINFILETIME;

#endif
