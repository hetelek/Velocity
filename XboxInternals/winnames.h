#pragma once

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

#else
// Define types for non-Windows platforms if necessary.
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef signed long long INT64;
typedef unsigned long long UINT64;
typedef signed int INT32;
#endif

typedef struct _WINFILETIME {
    DWORD dwHighDateTime;
    DWORD dwLowDateTime;
} WINFILETIME;

typedef short INT16;
typedef signed int INT24;
