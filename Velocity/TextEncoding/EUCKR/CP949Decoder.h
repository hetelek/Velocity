#ifndef CP949DECODER_H
#define CP949DECODER_H

#include "../TextDecoder.h"

namespace Velocity::TextEncoding {

/**
 * @brief Decoder for CP949 (EUC-KR - Korean encoding)
 * 
 * CP949 is Microsoft's implementation of EUC-KR with additional characters.
 * It's the standard encoding for Korean text on Xbox 360 and Windows systems.
 * 
 * Encoding structure:
 * - ASCII: 0x00-0x7F (single byte)
 * - Double-byte: lead byte (0xA1-0xFE) + trail byte (0xA1-0xFE)
 * 
 * This implementation uses lookup tables from CP949Mapping.h for bidirectional conversion.
 */
class CP949Decoder : public TextDecoder {
public:
    CP949Decoder() = default;
    ~CP949Decoder() override = default;

    /**
     * @brief Decode CP949-encoded bytes to Unicode string
     * @param data Raw byte sequence in CP949 encoding
     * @return Decoded QString (unmapped characters become U+FFFD)
     */
    QString decode(const QByteArray& data) const override;

    /**
     * @brief Encode Unicode string to CP949 bytes
     * @param text QString to encode
     * @return CP949-encoded byte sequence (unmapped characters become '?')
     */
    QByteArray encode(const QString& text) const override;

    /**
     * @brief Get decoder name
     * @return "CP949"
     */
    QString name() const override {
        return "CP949";
    }
    
    /**
     * @brief Check if a Unicode character can be encoded in CP949
     * @param ch Unicode character to test
     * @return true if character has a CP949 mapping
     */
    bool supports(QChar ch) const override;

    /**
     * @brief Check if a byte is a CP949 lead byte
     * @param byte Byte to check
     * @return true if byte is a valid CP949 lead byte
     */
    static bool isLeadByte(quint8 byte);

private:
    /**
     * @brief Lookup Unicode codepoint for CP949 double-byte character
     * @param lead Lead byte (0xA1-0xFE)
     * @param trail Trail byte (0xA1-0xFE)
     * @return Unicode codepoint, or 0 if not found
     */
    static quint16 lookupUnicode(quint8 lead, quint8 trail);

    /**
     * @brief Lookup CP949 encoding for Unicode codepoint
     * @param unicode Unicode codepoint
     * @param outLead Output parameter for lead byte (0 if single-byte)
     * @param outTrail Output parameter for trail byte
     * @return true if mapping found
     */
    static bool lookupCP949(quint16 unicode, quint8& outLead, quint8& outTrail);
};

} // namespace Velocity::TextEncoding

#endif // CP949DECODER_H
