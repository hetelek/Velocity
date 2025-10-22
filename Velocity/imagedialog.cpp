#include "imagedialog.h"
#include "ui_imagedialog.h"

ImageDialog::ImageDialog(QImage image, const QString &fileName, QWidget *parent) : QDialog(parent), ui(new Ui::ImageDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    
    // Set window title
    if (!fileName.isEmpty())
        setWindowTitle("Image Viewer - " + fileName);
    else
        setWindowTitle("Image Viewer");
    
    ui->label->setPixmap(QPixmap::fromImage(image));

    QSize s = sizeHint();
    if (s.width() < 80)
        s.setWidth(80);
    if (s.height() < 80)
        s.setHeight(80);

    // Make window resizable and set initial size to fit image
    resize(s);
}

ImageDialog::~ImageDialog()
{
    delete ui;
}


