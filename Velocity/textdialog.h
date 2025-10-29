#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>
#include <QByteArray>

namespace Ui {
class TextDialog;
}

class TextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextDialog(const QString &textContent, const QString &fileName = "", QWidget *parent = nullptr);
    explicit TextDialog(const QByteArray &rawData, const QString &fileName = "", QWidget *parent = nullptr);
    ~TextDialog();

private slots:
    void onEncodingChanged(int index);

private:
    void setupConnections();
    void updateTextWithEncoding(int encodingIndex);
    void setStatusMessage(const QString &message);
    
    Ui::TextDialog *ui;
    QByteArray rawData_;  // Store raw bytes for re-decoding
    int detectedEncodingIndex_;
};

#endif // TEXTDIALOG_H
