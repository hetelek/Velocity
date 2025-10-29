#ifndef CP950DECODER_H
#define CP950DECODER_H

#include "../TextDecoder.h"

namespace Velocity::TextEncoding {

/**
 * @brief Decoder for CP950 (Big5 - Traditional Chinese encoding)
 * 
 * CP950 is Microsoft's implementation of Big5 with additional characters.
 * It's the standard encoding for Traditional Chinese text on Xbox 360 and Windows systems.
 * 
 * Encoding structure:
 * - ASCII: 0x00-0x7F (single byte)
 * - Double-byte: lead byte (0xA1-0xF9) + trail byte (0x40-0x7E, 0x80-0xFE)
 * 
 * This implementation uses lookup tables from CP950Mapping.h for bidirectional conversion.
 */
class CP950Decoder : public TextDecoder {
public:
    CP950Decoder() = default;
    ~CP950Decoder() override = default;

    /**
     * @brief Decode CP950-encoded bytes to Unicode string
     * @param data Raw byte sequence in CP950 encoding
     * @return Decoded QString (unmapped characters become U+FFFD)
     */
    QString decode(const QByteArray& data) const override;

    /**
     * @brief Encode Unicode string to CP950 bytes
     * @param text QString to encode
     * @return CP950-encoded byte sequence (unmapped characters become '?')
     */
    QByteArray encode(const QString& text) const override;

    /**
     * @brief Get decoder name
     * @return "CP950"
     */
    QString name() const override {
        return "CP950";
    }
    
    /**
     * @brief Check if a Unicode character can be encoded in CP950
     * @param ch Unicode character to test
     * @return true if character has a CP950 mapping
     */
    bool supports(QChar ch) const override;

    /**
     * @brief Check if a byte is a CP950 lead byte
     * @param byte Byte to check
     * @return true if byte is a valid CP950 lead byte
     */
    static bool isLeadByte(quint8 byte);

private:
    /**
     * @brief Lookup Unicode codepoint for CP950 double-byte character
     * @param lead Lead byte (0xA1-0xF9)
     * @param trail Trail byte (0x40-0x7E, 0x80-0xFE)
     * @return Unicode codepoint, or 0 if not found
     */
    static quint16 lookupUnicode(quint8 lead, quint8 trail);

    /**
     * @brief Lookup CP950 encoding for Unicode codepoint
     * @param unicode Unicode codepoint
     * @param outLead Output parameter for lead byte (0 if single-byte)
     * @param outTrail Output parameter for trail byte
     * @return true if mapping found
     */
    static bool lookupCP950(quint16 unicode, quint8& outLead, quint8& outTrail);
};

} // namespace Velocity::TextEncoding

#endif // CP950DECODER_H
