#pragma once
#include <QtGlobal>
#include <QChar>

namespace Velocity::TextEncoding {

struct CP936MapEntry {
    quint16 code;
    char32_t unicode;
};

// Mapping table defined in CP936Mapping.cpp
extern const CP936MapEntry cp936ToUnicodeMap[];
extern const size_t cp936MapSize;

// Inline lookup function
inline QChar mapCP936ToUnicode(quint16 code) {
    // Binary search through the mapping table
    const CP936MapEntry* begin = cp936ToUnicodeMap;
    const CP936MapEntry* end = cp936ToUnicodeMap + cp936MapSize;
    
    const CP936MapEntry* it = std::lower_bound(
        begin, end, code,
        [](const CP936MapEntry &entry, quint16 key) { return entry.code < key; }
    );
    
    if (it != end && it->code == code)
        return QChar(it->unicode);
    return QChar(0xFFFD);
}

} // namespace Velocity::TextEncoding
