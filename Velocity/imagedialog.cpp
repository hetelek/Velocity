#include "imagedialog.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QImage image, QWidget *parent) : QDialog(parent), ui(new Ui::ImageDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    ui->label->setPixmap(QPixmap::fromImage(image));

    this->setFixedSize(this->sizeHint());
}

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::on_pushButton_clicked()
{
    this->close();
}
