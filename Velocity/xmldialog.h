#ifndef XMLDIALOG_H
#define XMLDIALOG_H

#include <QDialog>

namespace Ui {
class XmlDialog;
}

class XmlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XmlDialog(const QString &xmlContent, const QString &fileName = "", QWidget *parent = nullptr);
    ~XmlDialog();

private:
    Ui::XmlDialog *ui;
};

#endif // XMLDIALOG_H
