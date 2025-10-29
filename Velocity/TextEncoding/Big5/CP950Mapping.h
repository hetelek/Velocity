#pragma once
#include <QtGlobal>
#include <QChar>

namespace Velocity::TextEncoding {

struct CP950MapEntry {
    quint16 code;
    char32_t unicode;
};

// Mapping table defined in CP950Mapping.cpp
extern const CP950MapEntry cp950ToUnicodeMap[];
extern const size_t cp950MapSize;

// Inline lookup function
inline QChar mapCP950ToUnicode(quint16 code) {
    // Binary search through the mapping table
    const CP950MapEntry* begin = cp950ToUnicodeMap;
    const CP950MapEntry* end = cp950ToUnicodeMap + cp950MapSize;
    
    const CP950MapEntry* it = std::lower_bound(
        begin, end, code,
        [](const CP950MapEntry &entry, quint16 key) { return entry.code < key; }
    );
    
    if (it != end && it->code == code)
        return QChar(it->unicode);
    return QChar(0xFFFD);
}

} // namespace Velocity::TextEncoding
