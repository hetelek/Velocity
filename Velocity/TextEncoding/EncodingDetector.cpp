#include "EncodingDetector.h"
#include "ShiftJIS/CP932Decoder.h"
#include "GBK/CP936Decoder.h"
#include "Big5/CP950Decoder.h"
#include "EUCKR/CP949Decoder.h"
#include <algorithm>

namespace Velocity::TextEncoding {

EncodingDetector::Encoding EncodingDetector::detect(const QByteArray& data) {
    if (data.isEmpty()) {
        return Encoding::Unknown;
    }
    
    // Check for BOM first (most reliable)
    if (hasUTF8BOM(data)) {
        return Encoding::UTF8;
    }
    if (hasUTF16LEBOM(data)) {
        return Encoding::UTF16LE;
    }
    if (hasUTF16BEBOM(data)) {
        return Encoding::UTF16BE;
    }
    
    // Check for UTF-16 without BOM (null byte patterns)
    if (isLikelyUTF16LE(data)) {
        return Encoding::UTF16LE;
    }
    
    // Scan for high bytes (non-ASCII)
    const int scanSize = std::min<int>(data.size(), kMaxScanBytes);
    bool hasHighBytes = false;
    for (int i = 0; i < scanSize; ++i) {
        if (static_cast<quint8>(data[i]) >= 0x80) {
            hasHighBytes = true;
            break;
        }
    }
    
    if (!hasHighBytes) {
        // Pure ASCII - valid UTF-8
        return Encoding::UTF8;
    }
    
    // Try UTF-8 validation
    QString utf8Test = QString::fromUtf8(data);
    bool validUTF8 = !utf8Test.contains(QChar::ReplacementCharacter);
    
    if (validUTF8) {
        return Encoding::UTF8;
    }
    
    // Check legacy encodings with scoring system to handle overlapping byte ranges
    struct EncodingScore {
        Encoding encoding;
        int score;
    };
    
    EncodingScore scores[] = {
        {Encoding::CP932, 0},
        {Encoding::CP936, 0},
        {Encoding::CP950, 0},
        {Encoding::CP949, 0}
    };
    
    // Scan and score each encoding (reuse scanSize from above)
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data.constData());
    
    for (int i = 0; i < scanSize - 1; ++i) {
        const quint8 byte = p[i];
        const quint8 next = p[i + 1];
        
        if (byte < 0x80) continue; // Skip ASCII
        
        // CP932 (Shift-JIS): lead 0x81-0x9F, 0xE0-0xEF, 0xFA-0xFC
        if ((byte >= 0x81 && byte <= 0x9F) || (byte >= 0xE0 && byte <= 0xEF) || (byte >= 0xFA && byte <= 0xFC)) {
            if (next >= 0x40 && next <= 0xFC && next != 0x7F) {
                scores[0].score += 3; // Strong signal
            }
        }
        
        // CP936 (GBK): lead 0x81-0xFE, trail 0x40-0xFE
        if (byte >= 0x81 && byte <= 0xFE) {
            if (next >= 0x40 && next <= 0xFE && next != 0x7F) {
                scores[1].score += 2; // Medium signal (very broad range)
            }
        }
        
        // CP950 (Big5): lead 0xA1-0xF9, trail 0x40-0x7E or 0x80-0xFE
        if (byte >= 0xA1 && byte <= 0xF9) {
            if ((next >= 0x40 && next <= 0x7E) || (next >= 0x80 && next <= 0xFE)) {
                scores[2].score += 3; // Strong signal (narrower lead range)
            }
        }
        
        // CP949 (EUC-KR): lead 0xA1-0xFE, trail 0xA1-0xFE
        if (byte >= 0xA1 && byte <= 0xFE) {
            if (next >= 0xA1 && next <= 0xFE) {
                scores[3].score += 4; // Strongest signal (most restrictive)
            }
        }
    }
    
    // Find highest score above threshold
    int maxScore = 0;
    Encoding bestEncoding = Encoding::Latin1;
    
    for (const auto& s : scores) {
        if (s.score > maxScore && s.score >= 10) { // Minimum threshold
            maxScore = s.score;
            bestEncoding = s.encoding;
        }
    }
    
    if (maxScore >= 10) {
        return bestEncoding;
    }
    
    // Fallback to Latin1
    return Encoding::Latin1;
}

std::unique_ptr<TextDecoder> EncodingDetector::createDecoder(Encoding encoding) {
    switch (encoding) {
        case Encoding::CP932:
            return std::make_unique<CP932Decoder>();
        case Encoding::CP936:
            return std::make_unique<CP936Decoder>();
        case Encoding::CP950:
            return std::make_unique<CP950Decoder>();
        case Encoding::CP949:
            return std::make_unique<CP949Decoder>();
        
        default:
            return nullptr;
    }
}

QString EncodingDetector::encodingName(Encoding encoding) {
    switch (encoding) {
        case Encoding::UTF8:      return "UTF-8";
        case Encoding::UTF16LE:   return "UTF-16 LE";
        case Encoding::UTF16BE:   return "UTF-16 BE";
        case Encoding::UTF32LE:   return "UTF-32 LE";
        case Encoding::UTF32BE:   return "UTF-32 BE";
        case Encoding::CP932:     return "Shift-JIS (CP932)";
        case Encoding::CP936:     return "GBK (CP936)";
        case Encoding::CP950:     return "Big5 (CP950)";
        case Encoding::CP949:     return "EUC-KR (CP949)";
        case Encoding::Latin1:    return "ISO-8859-1 (Latin-1)";
        case Encoding::Unknown:   return "Unknown";
        default:                  return "Unknown";
    }
}

bool EncodingDetector::isSupported(Encoding encoding) {
    switch (encoding) {
        case Encoding::UTF8:
        case Encoding::UTF16LE:
        case Encoding::UTF16BE:
        case Encoding::Latin1:
        case Encoding::CP932:
        case Encoding::CP936:
        case Encoding::CP950:
        case Encoding::CP949:
            return true;
        
        // Not yet implemented:
        case Encoding::UTF32LE:
        case Encoding::UTF32BE:
        case Encoding::Unknown:
        default:
            return false;
    }
}

// BOM Detection

bool EncodingDetector::hasUTF8BOM(const QByteArray& data) {
    return data.size() >= 3 &&
           static_cast<quint8>(data[0]) == 0xEF &&
           static_cast<quint8>(data[1]) == 0xBB &&
           static_cast<quint8>(data[2]) == 0xBF;
}

bool EncodingDetector::hasUTF16LEBOM(const QByteArray& data) {
    return data.size() >= 2 &&
           static_cast<quint8>(data[0]) == 0xFF &&
           static_cast<quint8>(data[1]) == 0xFE;
}

bool EncodingDetector::hasUTF16BEBOM(const QByteArray& data) {
    return data.size() >= 2 &&
           static_cast<quint8>(data[0]) == 0xFE &&
           static_cast<quint8>(data[1]) == 0xFF;
}

bool EncodingDetector::isLikelyUTF16LE(const QByteArray& data) {
    if (data.size() < 100) {
        return false;
    }
    
    // Check for null bytes at odd positions (UTF-16LE ASCII pattern)
    // Also verify that even positions have few nulls
    int oddNullCount = 0;
    int evenNullCount = 0;
    
    for (int i = 0; i < 100 && i < data.size(); ++i) {
        if (data[i] == '\0') {
            if (i % 2 == 1) {
                ++oddNullCount;
            } else {
                ++evenNullCount;
            }
        }
    }
    
    // UTF-16LE: >40% of odd-indexed bytes are null AND <10% of even-indexed bytes are null
    return (oddNullCount > 20) && (evenNullCount < 5);
}

// Legacy Encoding Detection

bool EncodingDetector::isLikelyCP932(const QByteArray& data) {
    const int scanSize = std::min<int>(data.size(), kMaxScanBytes);
    int leadByteCount = 0;
    int highByteCount = 0;
    
    for (int i = 0; i < scanSize; ++i) {
        const quint8 byte = static_cast<quint8>(data[i]);
        
        if (byte >= 0x80) {
            ++highByteCount;
            
            // CP932 lead byte ranges: 0x81-0x9F, 0xE0-0xEF, 0xFA-0xFC
            if ((byte >= 0x81 && byte <= 0x9F) ||
                (byte >= 0xE0 && byte <= 0xEF) ||
                (byte >= 0xFA && byte <= 0xFC)) {
                ++leadByteCount;
            }
        }
    }
    
    // Heuristic: If >10% of high bytes are CP932 lead bytes, likely CP932
    return highByteCount > 0 &&
           (leadByteCount * 100 / highByteCount) > 10;
}

bool EncodingDetector::isLikelyCP936(const QByteArray& data) {
    const int scanSize = std::min<int>(data.size(), kMaxScanBytes);
    int leadByteCount = 0;
    int highByteCount = 0;
    
    for (int i = 0; i < scanSize; ++i) {
        const quint8 byte = static_cast<quint8>(data[i]);
        
        if (byte >= 0x80) {
            ++highByteCount;
            
            // GBK/CP936 lead byte range: 0x81-0xFE
            if (byte >= 0x81 && byte <= 0xFE) {
                ++leadByteCount;
            }
        }
    }
    
    // Heuristic: If >15% of high bytes are GBK lead bytes, likely CP936
    return highByteCount > 0 &&
           (leadByteCount * 100 / highByteCount) > 15;
}

bool EncodingDetector::isLikelyCP950(const QByteArray& data) {
    const int scanSize = std::min<int>(data.size(), kMaxScanBytes);
    int leadByteCount = 0;
    int highByteCount = 0;
    
    for (int i = 0; i < scanSize; ++i) {
        const quint8 byte = static_cast<quint8>(data[i]);
        
        if (byte >= 0x80) {
            ++highByteCount;
            
            // Big5/CP950 lead byte ranges: 0xA1-0xF9 (similar to GBK but different trails)
            if (byte >= 0xA1 && byte <= 0xF9) {
                ++leadByteCount;
            }
        }
    }
    
    // Heuristic: If >15% of high bytes are Big5 lead bytes, likely CP950
    return highByteCount > 0 &&
           (leadByteCount * 100 / highByteCount) > 15;
}

bool EncodingDetector::isLikelyCP949(const QByteArray& data) {
    const int scanSize = std::min<int>(data.size(), kMaxScanBytes);
    int leadByteCount = 0;
    int highByteCount = 0;
    
    for (int i = 0; i < scanSize; ++i) {
        const quint8 byte = static_cast<quint8>(data[i]);
        
        if (byte >= 0x80) {
            ++highByteCount;
            
            // EUC-KR/CP949 lead byte range: 0xA1-0xFE
            if (byte >= 0xA1 && byte <= 0xFE) {
                ++leadByteCount;
            }
        }
    }
    
    // Heuristic: If >15% of high bytes are CP949 lead bytes, likely CP949
    return highByteCount > 0 &&
           (leadByteCount * 100 / highByteCount) > 15;
}

} // namespace Velocity::TextEncoding
