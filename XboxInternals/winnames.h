#ifndef TYPEDEFINITIONS_H
#define TYPEDEFINITIONS_H

#include <cstdint>

// Fixed-width types for cross-platform compatibility
using BYTE = std::uint8_t;
using WORD = std::uint16_t;
using DWORD = std::uint32_t;
using INT16 = std::int16_t;
using INT32 = std::int32_t;
using INT64 = std::int64_t;
using UINT64 = std::uint64_t;

// 24-bit integer emulation for STFS format (stored in 32-bit, masked to 24-bit)
using INT24 = std::int32_t;   // Signed 24-bit: -8,388,608 to 8,388,607
using UINT24 = std::uint32_t;  // Unsigned 24-bit: 0 to 16,777,215

// 24-bit constants for STFS block management
constexpr INT24  INT24_MAX = 0x7FFFFF;     // Max signed 24-bit value
constexpr INT24  INT24_MIN = -0x800000;    // Min signed 24-bit value  
constexpr UINT24 UINT24_MAX = 0xFFFFFF;     // Max unsigned 24-bit value
constexpr UINT24 BLOCK_CHAIN_TERMINATOR = 0xFFFFFF; // STFS block chain end marker
constexpr UINT24 MASK_24BIT = 0xFFFFFF;     // Bit mask for 24-bit values

// Usage: UINT24 for block numbers, INT24 for signed data. Always mask with MASK_24BIT.

// Windows FILETIME structure
struct WINFILETIME {
    DWORD dwHighDateTime;
    DWORD dwLowDateTime;
};

// Compile-time size verification
static_assert(sizeof(BYTE) == 1, "BYTE must be 1 byte");
static_assert(sizeof(WORD) == 2, "WORD must be 2 bytes");
static_assert(sizeof(DWORD) == 4, "DWORD must be 4 bytes");
static_assert(sizeof(INT32) == 4, "INT32 must be 4 bytes");
static_assert(sizeof(INT64) == 8, "INT64 must be 8 bytes");
static_assert(sizeof(WINFILETIME) == 8, "WINFILETIME must be 8 bytes");

#endif // TYPEDEFINITIONS_H

