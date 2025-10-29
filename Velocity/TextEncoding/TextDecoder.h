#pragma once

#include <QString>
#include <QByteArray>

namespace Velocity::TextEncoding {

/**
 * @brief Common interface for all text encoding decoders
 * 
 * Provides a uniform API for converting legacy byte encodings to Unicode
 * strings. Implementations live in subdirectories (CP932/, GBK/, etc.)
 * to keep the codebase organized and modular.
 * 
 * Each decoder handles a specific character encoding used in Xbox 360
 * game files (scripts, config files, localized text assets).
 */
class TextDecoder {
public:
    virtual ~TextDecoder() = default;
    
    /**
     * @brief Decode byte array to Unicode string
     * @param data Raw encoded bytes
     * @return Decoded string (unmapped characters become U+FFFD)
     * @note Decoders are stateless and thread-safe. Safe for parallel use.
     */
    [[nodiscard]] virtual QString decode(const QByteArray& data) const = 0;
    
    /**
     * @brief Encode Unicode string to byte array
     * @param text Unicode string
     * @return Encoded bytes (unmappable characters become '?')
     * @note Decoders are stateless and thread-safe. Safe for parallel use.
     */
    [[nodiscard]] virtual QByteArray encode(const QString& text) const = 0;
    
    /**
     * @brief Get human-readable encoding name
     * @return Display name (e.g., "Shift-JIS (CP932)")
     */
    virtual QString name() const = 0;
    
    /**
     * @brief Check if a Unicode character can be encoded
     * @param ch Unicode character to test
     * @return true if character has a mapping in this encoding
     * @note Useful for UI input validation and encoding compatibility checks
     */
    [[nodiscard]] virtual bool supports(QChar ch) const = 0;
};

} // namespace Velocity::TextEncoding
