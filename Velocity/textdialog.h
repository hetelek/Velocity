#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>

namespace Ui {
class TextDialog;
}

class TextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextDialog(const QString &textContent, const QString &fileName = "", QWidget *parent = nullptr);
    ~TextDialog();

private:
    Ui::TextDialog *ui;
};

#endif // TEXTDIALOG_H
