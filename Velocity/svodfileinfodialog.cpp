#include "svodfileinfodialog.h"
#include "ui_svodfileinfodialog.h"

SvodFileInfoDialog::SvodFileInfoDialog(GDFXFileEntry *entry, QWidget *parent) :
    QDialog(parent), ui(new Ui::SvodFileInfoDialog), entry(entry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // load the information
    ui->lblLocation->setText(QString::fromStdString(entry->filePath));
    ui->txtName->setText(QString::fromStdString(entry->name));
    ui->lblSize->setText(QString::fromStdString(ByteSizeToString(entry->size)) + " (" + QString::number(entry->size) + ")");

    DWORD sizeOnDisk = (entry->size + 0x7FF) & 0xFFFFF800;
    ui->lblSizeOnDisk->setText(QString::fromStdString(ByteSizeToString(sizeOnDisk)) + " (" + QString::number(sizeOnDisk) + ")");

    ui->lblTypeOfFile->setText(getFileType(QString::fromStdString(entry->name)));

    // load flags
    ui->chArchive->setChecked(entry->attributes & GdfxArchive);
    ui->chDevice->setChecked(entry->attributes & GdfxDevice);
    ui->chDirectory->setChecked(entry->attributes & GdfxDirectory);
    ui->chHidden->setChecked(entry->attributes & GdfxHidden);
    ui->chNormal->setChecked(entry->attributes & GdfxNormal);
    ui->chReadOnly->setChecked(entry->attributes & GdfxReadOnly);
    ui->chSystem->setChecked(entry->attributes & GdfxSystem);
}

SvodFileInfoDialog::~SvodFileInfoDialog()
{
    delete ui;
}

QString SvodFileInfoDialog::getFileType(QString fileName)
{
    QString extension = fileName.mid(fileName.lastIndexOf(".") + 1);
    if (extension == fileName)
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/DefaultFileIcon.png"));
        return "FILE";
    }

    if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/ImageFileIcon.png"));
        return "Image (" + extension + ")";
    }
    else if (extension == "xex")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/XEXFileIcon.png"));
        return "Xenon Executable (" + extension + ")";
    }
    else
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/DefaultFileIcon.png"));
        return extension.toUpper() + "(" + extension + ")";
    }
}

void SvodFileInfoDialog::on_pushButton_clicked()
{
    close();
}

void SvodFileInfoDialog::on_pushButton_2_clicked()
{
    close();
}

void SvodFileInfoDialog::on_pushButton_3_clicked()
{
    close();
}
