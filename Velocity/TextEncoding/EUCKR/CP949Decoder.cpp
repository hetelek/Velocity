#include "CP949Decoder.h"
#include "CP949Mapping.h"
#include <QDebug>
#include <QHash>
#include <QMutex>

namespace Velocity::TextEncoding {

// Reverse lookup cache: Unicode → CP949 code (lazy initialized)
static QHash<char32_t, quint16> g_unicodeToCP949Cache;
static QMutex g_cacheMutex;
static bool g_cacheInitialized = false;

static void initializeReverseCache() {
    QMutexLocker locker(&g_cacheMutex);
    
    if (g_cacheInitialized)
        return;
    
    // Build hash map from mapping table (one-time cost)
    g_unicodeToCP949Cache.reserve(cp949MapSize);
    for (size_t i = 0; i < cp949MapSize; ++i) {
        g_unicodeToCP949Cache.insert(
            cp949ToUnicodeMap[i].unicode,
            cp949ToUnicodeMap[i].code
        );
    }
    
    g_cacheInitialized = true;
}

QString CP949Decoder::decode(const QByteArray& data) const {
    QString result;
    result.reserve(data.size()); // Optimize allocation
    
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data.constData());
    const int size = data.size();
    
    int i = 0;
    while (i < size) {
        const quint8 byte = p[i];
        
        // ASCII fast path (0x00-0x7F)
        if (byte < 0x80) {
            result.append(QChar(byte));
            ++i;
            continue;
        }
        
        // Check for lead byte
        if (isLeadByte(byte)) {
            // Need trail byte
            if (i + 1 >= size) {
                // Orphan lead byte at EOF - emit replacement character
                result.append(QChar::ReplacementCharacter);
                ++i;
                break;
            }
            
            const quint8 trail = p[i + 1];
            
            // Lookup in mapping table
            const quint16 unicode = lookupUnicode(byte, trail);
            
            if (unicode != 0) {
                result.append(QChar(unicode));
            } else {
                // Invalid sequence - append replacement character
                result.append(QChar::ReplacementCharacter);
            }
            
            i += 2; // Skip both bytes
        } else {
            // Invalid single byte (0x80-0xA0 without valid trail)
            result.append(QChar::ReplacementCharacter);
            ++i;
        }
    }
    
    return result;
}

QByteArray CP949Decoder::encode(const QString& text) const {
    QByteArray result;
    result.reserve(text.size() * 2); // Worst case: all double-byte
    
    for (const QChar& ch : text) {
        const quint16 unicode = ch.unicode();
        
        // ASCII fast path
        if (unicode < 0x80) {
            result.append(static_cast<char>(unicode));
            continue;
        }
        
        // Lookup in mapping table
        quint8 lead = 0, trail = 0;
        if (lookupCP949(unicode, lead, trail)) {
            if (lead == 0) {
                // Single-byte character
                result.append(static_cast<char>(trail));
            } else {
                // Double-byte character
                result.append(static_cast<char>(lead));
                result.append(static_cast<char>(trail));
            }
        } else {
            // Unmapped character - use '?'
            result.append('?');
        }
    }
    
    return result;
}

bool CP949Decoder::isLeadByte(quint8 byte) {
    // CP949/EUC-KR lead byte range: 0xA1-0xFE
    return byte >= 0xA1 && byte <= 0xFE;
}

quint16 CP949Decoder::lookupUnicode(quint8 lead, quint8 trail) {
    const quint16 code = (static_cast<quint16>(lead) << 8) | trail;
    
    // Use the mapping table's helper function
    QChar mapped = mapCP949ToUnicode(code);
    
    if (mapped == QChar(0xFFFD)) {
        return 0; // Not found
    }
    
    return mapped.unicode();
}

bool CP949Decoder::lookupCP949(quint16 unicode, quint8& outLead, quint8& outTrail) {
    // Initialize reverse lookup cache on first use
    if (!g_cacheInitialized) {
        initializeReverseCache();
    }
    
    // O(1) hash lookup instead of O(n) linear search
    auto it = g_unicodeToCP949Cache.constFind(unicode);
    if (it == g_unicodeToCP949Cache.constEnd()) {
        return false;
    }
    
    const quint16 code = it.value();
    
    if (code < 0x100) {
        // Single-byte character
        outLead = 0;
        outTrail = static_cast<quint8>(code);
    } else {
        // Double-byte character
        outLead = static_cast<quint8>(code >> 8);
        outTrail = static_cast<quint8>(code & 0xFF);
    }
    
    return true;
}

bool CP949Decoder::supports(QChar ch) const {
    const quint16 unicode = ch.unicode();
    
    // ASCII is always supported
    if (unicode < 0x80)
        return true;
    
    // Check if we can encode this character
    quint8 lead = 0, trail = 0;
    return lookupCP949(unicode, lead, trail);
}

} // namespace Velocity::TextEncoding
