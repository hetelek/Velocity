#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include <QImage>

namespace Ui
{
class ImageDialog;
}

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(QImage image, const QString &fileName = "", QWidget *parent = 0);
    ~ImageDialog();

private:
    Ui::ImageDialog *ui;
};

#endif // IMAGEDIALOG_H


