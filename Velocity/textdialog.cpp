#include "textdialog.h"
#include "ui_textdialog.h"
#include "TextEncoding/EncodingDetector.h"
#include <QStringDecoder>

TextDialog::TextDialog(const QString &textContent, const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextDialog),
    detectedEncodingIndex_(0)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    // Set window title
    if (!fileName.isEmpty())
        setWindowTitle("Text Viewer - " + fileName);
    else
        setWindowTitle("Text Viewer");
    
    // Set the text content in the text edit
    ui->textEdit->setPlainText(textContent);
    
    // Make it read-only
    ui->textEdit->setReadOnly(true);
    
    // Use a monospace font for better readability
    QFont font("Courier New", 10);
    ui->textEdit->setFont(font);
    
    // Hide encoding controls when constructed with QString
    ui->encodingLabel->setVisible(false);
    ui->encodingComboBox->setVisible(false);
    ui->statusLabel->setVisible(false);
}

TextDialog::TextDialog(const QByteArray &rawData, const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextDialog),
    rawData_(rawData),
    detectedEncodingIndex_(0)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    // Set window title
    if (!fileName.isEmpty())
        setWindowTitle("Text Viewer - " + fileName);
    else
        setWindowTitle("Text Viewer");
    
    // Use a monospace font for better readability
    QFont font("Courier New", 10);
    ui->textEdit->setFont(font);
    
    // Auto-detect encoding and populate combo box
    auto encoding = Velocity::TextEncoding::EncodingDetector::detect(rawData_);
    
    // Map encoding to combo box index
    using Enc = Velocity::TextEncoding::EncodingDetector::Encoding;
    switch (encoding) {
        case Enc::CP932:   detectedEncodingIndex_ = 1; break;
        case Enc::CP936:   detectedEncodingIndex_ = 2; break;
        case Enc::CP950:   detectedEncodingIndex_ = 3; break;
        case Enc::CP949:   detectedEncodingIndex_ = 4; break;
        case Enc::UTF8:    detectedEncodingIndex_ = 5; break;
        case Enc::UTF16LE: detectedEncodingIndex_ = 6; break;
        case Enc::UTF16BE: detectedEncodingIndex_ = 7; break;
        case Enc::Latin1:  detectedEncodingIndex_ = 8; break;
        default:           detectedEncodingIndex_ = 0; break; // Auto-detect
    }
    
    // Set combo box to detected encoding
    ui->encodingComboBox->setCurrentIndex(detectedEncodingIndex_);
    
    // Add tooltips for better user experience
    ui->encodingComboBox->setToolTip(
        "Select character encoding for this file.\n"
        "Auto-detect uses heuristics to determine the best encoding.\n"
        "If text appears garbled, try different encodings manually."
    );
    ui->textEdit->setToolTip(
        "Read-only text view with encoding support.\n"
        "Change encoding using the dropdown above if text is garbled."
    );
    
    // Decode with detected encoding
    updateTextWithEncoding(detectedEncodingIndex_);
    
    // Set status message with confidence indicator
    QString encodingName = Velocity::TextEncoding::EncodingDetector::encodingName(encoding);
    setStatusMessage(QString("Auto-detected: %1").arg(encodingName));
    
    // Connect signals
    setupConnections();
}

TextDialog::~TextDialog()
{
    delete ui;
}

void TextDialog::setupConnections()
{
    connect(ui->encodingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TextDialog::onEncodingChanged);
}

void TextDialog::onEncodingChanged(int index)
{
    updateTextWithEncoding(index);
    
    // Update status message
    if (index == 0) {
        // Reset to auto-detect
        ui->encodingComboBox->setCurrentIndex(detectedEncodingIndex_);
        return;
    }
    
    if (index == detectedEncodingIndex_) {
        QString encodingName = ui->encodingComboBox->currentText();
        setStatusMessage(QString("Auto-detected: %1").arg(encodingName));
    } else {
        QString encodingName = ui->encodingComboBox->currentText();
        setStatusMessage(QString("Manual override: %1").arg(encodingName));
    }
}

void TextDialog::updateTextWithEncoding(int encodingIndex)
{
    using Enc = Velocity::TextEncoding::EncodingDetector::Encoding;
    
    QString textContent;
    Enc encoding;
    
    // Map combo box index to encoding
    switch (encodingIndex) {
        case 1: encoding = Enc::CP932;   break;
        case 2: encoding = Enc::CP936;   break;
        case 3: encoding = Enc::CP950;   break;
        case 4: encoding = Enc::CP949;   break;
        case 5: encoding = Enc::UTF8;    break;
        case 6: encoding = Enc::UTF16LE; break;
        case 7: encoding = Enc::UTF16BE; break;
        case 8: encoding = Enc::Latin1;  break;
        default:
            // Auto-detect
            encoding = Velocity::TextEncoding::EncodingDetector::detect(rawData_);
            break;
    }
    
    // Check for empty data
    if (rawData_.isEmpty()) {
        ui->textEdit->setPlainText("(Empty file)");
        setStatusMessage("No data to decode");
        return;
    }
    
    // Decode using selected encoding
    auto decoder = Velocity::TextEncoding::EncodingDetector::createDecoder(encoding);
    
    if (decoder) {
        // Use our custom decoder (CP932, CP936, CP950, CP949)
        textContent = decoder->decode(rawData_);
        
        // Check for decoding issues
        int replacementCount = textContent.count(QChar::ReplacementCharacter);
        if (replacementCount > 0 && replacementCount > textContent.length() / 10) {
            // More than 10% replacement characters suggests wrong encoding
            QString currentStatus = ui->statusLabel->text();
            setStatusMessage(currentStatus + QString(" ⚠ %1 unmapped characters - try different encoding")
                           .arg(replacementCount));
        }
    } else {
        // Fall back to Qt's built-in decoders
        switch (encoding) {
            case Enc::UTF8: {
                auto toUtf8 = QStringDecoder(QStringDecoder::Utf8);
                textContent = toUtf8(rawData_);
                if (!toUtf8.hasError()) {
                    // Successfully decoded
                } else {
                    setStatusMessage("UTF-8 decoding failed - file may be corrupted");
                    textContent = QString::fromLatin1(rawData_); // Fallback
                }
                break;
            }
            case Enc::UTF16LE: {
                auto toUtf16LE = QStringDecoder(QStringDecoder::Utf16LE);
                textContent = toUtf16LE(rawData_);
                break;
            }
            case Enc::UTF16BE: {
                auto toUtf16BE = QStringDecoder(QStringDecoder::Utf16BE);
                textContent = toUtf16BE(rawData_);
                break;
            }
            case Enc::Latin1:
            default:
                textContent = QString::fromLatin1(rawData_);
                break;
        }
    }
    
    ui->textEdit->setPlainText(textContent);
}

void TextDialog::setStatusMessage(const QString &message)
{
    ui->statusLabel->setText(message);
}
