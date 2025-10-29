#pragma once
#include <QtGlobal>
#include <QChar>

namespace Velocity::TextEncoding {

struct CP949MapEntry {
    quint16 code;
    char32_t unicode;
};

// Mapping table defined in CP949Mapping.cpp
extern const CP949MapEntry cp949ToUnicodeMap[];
extern const size_t cp949MapSize;

// Inline lookup function
inline QChar mapCP949ToUnicode(quint16 code) {
    // Binary search through the mapping table
    const CP949MapEntry* begin = cp949ToUnicodeMap;
    const CP949MapEntry* end = cp949ToUnicodeMap + cp949MapSize;
    
    const CP949MapEntry* it = std::lower_bound(
        begin, end, code,
        [](const CP949MapEntry &entry, quint16 key) { return entry.code < key; }
    );
    
    if (it != end && it->code == code)
        return QChar(it->unicode);
    return QChar(0xFFFD);
}

} // namespace Velocity::TextEncoding
