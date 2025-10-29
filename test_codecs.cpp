#include <QCoreApplication>
#include <QStringConverter>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    std::cout << "=== Available Qt String Codecs ===" << std::endl;
    std::cout << std::endl;
    
    QStringList codecs = QStringConverter::availableCodecs();
    
    if (codecs.isEmpty()) {
        std::cout << "No codecs available!" << std::endl;
        std::cout << "Qt 6 requires ICU libraries for additional codecs." << std::endl;
    } else {
        std::cout << "Found " << codecs.size() << " codec(s):" << std::endl;
        std::cout << std::endl;
        
        for (const QString& codec : codecs) {
            std::cout << "  - " << codec.toStdString() << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Testing Specific Codecs ===" << std::endl;
    std::cout << std::endl;
    
    // Test common encodings
    QStringList testEncodings = {
        "UTF-8",
        "UTF-16",
        "UTF-16BE",
        "UTF-16LE",
        "UTF-32",
        "UTF-32BE",
        "UTF-32LE",
        "ISO-8859-1",
        "Latin1",
        "System",
        "Shift-JIS",
        "Shift_JIS",
        "SJIS",
        "Windows-31J",
        "CP932",
        "EUC-JP",
        "ISO-2022-JP",
        "Big5",
        "GBK",
        "GB18030",
        "EUC-KR",
        "Windows-1252",
        "Windows-1251"
    };
    
    for (const QString& encoding : testEncodings) {
        auto decoder = QStringDecoder(encoding.toUtf8().constData());
        if (decoder.isValid()) {
            std::cout << "  ✓ " << encoding.toStdString() << " - AVAILABLE" << std::endl;
        } else {
            std::cout << "  ✗ " << encoding.toStdString() << " - NOT AVAILABLE" << std::endl;
        }
    }
    
    return 0;
}
