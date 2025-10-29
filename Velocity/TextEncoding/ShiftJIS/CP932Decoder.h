#ifndef CP932DECODER_H
#define CP932DECODER_H

#include "../TextDecoder.h"

namespace Velocity::TextEncoding {

/**
 * @brief Decoder for CP932 (Microsoft's extended Shift-JIS encoding)
 * 
 * CP932 is Microsoft's implementation of Shift-JIS with additional characters.
 * It's the standard encoding for Japanese text on Xbox 360 and Windows systems.
 * 
 * Encoding structure:
 * - ASCII: 0x00-0x7F (single byte)
 * - Half-width Katakana: 0xA1-0xDF (single byte)
 * - Double-byte: lead byte (0x81-0x9F, 0xE0-0xEF, 0xFA-0xFC) + trail byte (0x40-0xFC)
 * 
 * This implementation uses lookup tables from CP932Mapping.h for bidirectional conversion.
 */
class CP932Decoder : public TextDecoder {
public:
    CP932Decoder() = default;
    ~CP932Decoder() override = default;

    /**
     * @brief Decode CP932-encoded bytes to Unicode string
     * @param data Raw byte sequence in CP932 encoding
     * @return Decoded QString (unmapped characters become U+FFFD)
     */
    QString decode(const QByteArray& data) const override;

    /**
     * @brief Encode Unicode string to CP932 bytes
     * @param text QString to encode
     * @return CP932-encoded byte sequence (unmapped characters become '?')
     */
    QByteArray encode(const QString& text) const override;

    /**
     * @brief Get decoder name
     * @return "CP932"
     */
    QString name() const override {
        return "CP932";
    }
    
    /**
     * @brief Check if a Unicode character can be encoded in CP932
     * @param ch Unicode character to test
     * @return true if character has a CP932 mapping
     */
    bool supports(QChar ch) const override;

    /**
     * @brief Check if a byte is a CP932 lead byte
     * @param byte Byte to check
     * @return true if byte is a valid CP932 lead byte
     */
    static bool isLeadByte(quint8 byte);

private:
    /**
     * @brief Lookup Unicode codepoint for CP932 double-byte character
     * @param lead Lead byte (0x81-0x9F, 0xE0-0xEF, 0xFA-0xFC)
     * @param trail Trail byte (0x40-0xFC)
     * @return Unicode codepoint, or 0 if not found
     */
    static quint16 lookupUnicode(quint8 lead, quint8 trail);

    /**
     * @brief Lookup CP932 encoding for Unicode codepoint
     * @param unicode Unicode codepoint
     * @param outLead Output parameter for lead byte (0 if single-byte)
     * @param outTrail Output parameter for trail byte
     * @return true if mapping found
     */
    static bool lookupCP932(quint16 unicode, quint8& outLead, quint8& outTrail);
};

} // namespace Velocity::TextEncoding

#endif // CP932DECODER_H
