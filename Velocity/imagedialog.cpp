#include "imagedialog.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QImage image, QWidget *parent) : QDialog(parent), ui(new Ui::ImageDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    ui->label->setPixmap(QPixmap::fromImage(image));

    QSize s = sizeHint();
    if (s.width() < 80)
        s.setWidth(80);
    if (s.height() < 80)
        s.setHeight(80);

    setFixedSize(s);
}

ImageDialog::~ImageDialog()
{
    delete ui;
}
