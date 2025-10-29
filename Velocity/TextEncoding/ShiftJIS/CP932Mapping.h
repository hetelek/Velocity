#pragma once
#include <QtGlobal>
#include <QChar>

namespace Velocity::TextEncoding {

struct CP932MapEntry {
    quint16 code;
    char32_t unicode;
};

// Mapping table defined in CP932Mapping.cpp
extern const CP932MapEntry cp932ToUnicodeMap[];
extern const size_t cp932MapSize;

// Inline lookup function
inline QChar mapCP932ToUnicode(quint16 code) {
    // Binary search through the mapping table
    const CP932MapEntry* begin = cp932ToUnicodeMap;
    const CP932MapEntry* end = cp932ToUnicodeMap + cp932MapSize;
    
    const CP932MapEntry* it = std::lower_bound(
        begin, end, code,
        [](const CP932MapEntry &entry, quint16 key) { return entry.code < key; }
    );
    
    if (it != end && it->code == code)
        return QChar(it->unicode);
    return QChar(0xFFFD);
}

} // namespace Velocity::TextEncoding
