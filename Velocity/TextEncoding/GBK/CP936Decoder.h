#ifndef CP936DECODER_H
#define CP936DECODER_H

#include "../TextDecoder.h"

namespace Velocity::TextEncoding {

/**
 * @brief Decoder for CP936 (GBK - Simplified Chinese encoding)
 * 
 * CP936 is Microsoft's implementation of GBK with additional characters.
 * It's the standard encoding for Simplified Chinese text on Xbox 360 and Windows systems.
 * 
 * Encoding structure:
 * - ASCII: 0x00-0x7F (single byte)
 * - Double-byte: lead byte (0x81-0xFE) + trail byte (0x40-0xFE)
 * 
 * This implementation uses lookup tables from CP936Mapping.h for bidirectional conversion.
 */
class CP936Decoder : public TextDecoder {
public:
    CP936Decoder() = default;
    ~CP936Decoder() override = default;

    /**
     * @brief Decode CP936-encoded bytes to Unicode string
     * @param data Raw byte sequence in CP936 encoding
     * @return Decoded QString (unmapped characters become U+FFFD)
     */
    QString decode(const QByteArray& data) const override;

    /**
     * @brief Encode Unicode string to CP936 bytes
     * @param text QString to encode
     * @return CP936-encoded byte sequence (unmapped characters become '?')
     */
    QByteArray encode(const QString& text) const override;

    /**
     * @brief Get decoder name
     * @return "CP936"
     */
    QString name() const override {
        return "CP936";
    }
    
    /**
     * @brief Check if a Unicode character can be encoded in CP936
     * @param ch Unicode character to test
     * @return true if character has a CP936 mapping
     */
    bool supports(QChar ch) const override;

    /**
     * @brief Check if a byte is a CP936 lead byte
     * @param byte Byte to check
     * @return true if byte is a valid CP936 lead byte
     */
    static bool isLeadByte(quint8 byte);

private:
    /**
     * @brief Lookup Unicode codepoint for CP936 double-byte character
     * @param lead Lead byte (0x81-0xFE)
     * @param trail Trail byte (0x40-0xFE)
     * @return Unicode codepoint, or 0 if not found
     */
    static quint16 lookupUnicode(quint8 lead, quint8 trail);

    /**
     * @brief Lookup CP936 encoding for Unicode codepoint
     * @param unicode Unicode codepoint
     * @param outLead Output parameter for lead byte (0 if single-byte)
     * @param outTrail Output parameter for trail byte
     * @return true if mapping found
     */
    static bool lookupCP936(quint16 unicode, quint8& outLead, quint8& outTrail);
};

} // namespace Velocity::TextEncoding

#endif // CP936DECODER_H
