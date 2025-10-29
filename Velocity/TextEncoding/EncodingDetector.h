#pragma once

#include "TextDecoder.h"
#include <QString>
#include <QByteArray>
#include <memory>

namespace Velocity::TextEncoding {

/**
 * @brief Auto-detects text encoding from byte patterns
 * 
 * Analyzes byte sequences to identify likely encodings using:
 * - BOM (Byte Order Mark) detection for Unicode variants
 * - Statistical byte range analysis for legacy encodings
 * - Multi-byte sequence pattern matching
 * 
 * Detection scans the first 8KB of data for performance, which
 * is sufficient for most Xbox 360 game text files.
 */
class EncodingDetector {
public:
    enum class Encoding {
        Unknown,
        UTF8,
        UTF16LE,
        UTF16BE,
        UTF32LE,
        UTF32BE,
        CP932,      // Shift-JIS (Japanese) - Xbox 360 SDK standard
        CP936,      // GBK (Simplified Chinese) - future
        CP950,      // Big5 (Traditional Chinese) - future
        CP949,      // EUC-KR (Korean) - future
        Latin1      // ISO-8859-1 fallback
    };
    
    /**
     * @brief Detect encoding from byte array
     * @param data Byte array to analyze (checks first 8KB)
     * @return Detected encoding (returns Unknown if inconclusive)
     */
    static Encoding detect(const QByteArray& data);
    
    /**
     * @brief Create decoder for detected encoding
     * @param encoding Detected encoding type
     * @return Smart pointer to decoder (nullptr if unsupported)
     */
    static std::unique_ptr<TextDecoder> createDecoder(Encoding encoding);
    
    /**
     * @brief Get human-readable name for encoding
     * @param encoding Encoding type
     * @return Display name for UI (e.g., "Shift-JIS (CP932)")
     */
    static QString encodingName(Encoding encoding);
    
    /**
     * @brief Check if encoding is currently supported
     * @param encoding Encoding type to check
     * @return true if decoder implementation exists
     */
    static bool isSupported(Encoding encoding);

private:
    static constexpr int kMaxScanBytes = 8192;
    
    static bool hasUTF8BOM(const QByteArray& data);
    static bool hasUTF16LEBOM(const QByteArray& data);
    static bool hasUTF16BEBOM(const QByteArray& data);
    static bool isLikelyUTF16LE(const QByteArray& data);
    static bool isLikelyCP932(const QByteArray& data);
    static bool isLikelyCP936(const QByteArray& data);
    static bool isLikelyCP950(const QByteArray& data);
    static bool isLikelyCP949(const QByteArray& data);
};

} // namespace Velocity::TextEncoding
