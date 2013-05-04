#ifndef WINNAMES_H
#define WINNAMES_H

#ifdef _WIN32
    #include <windows.h>
#else
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int INT32;
typedef long long INT64;
typedef unsigned long long UINT64;
#endif

typedef struct _WINFILETIME
{
    DWORD dwHighDateTime;
    DWORD dwLowDateTime;

} WINFILETIME;

typedef short INT16;
typedef signed int INT24;

#endif
