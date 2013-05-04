#include "svodfileinfodialog.h"
#include "ui_svodfileinfodialog.h"

SvodFileInfoDialog::SvodFileInfoDialog(SVOD *svod, GdfxFileEntry *entry, QString type, QWidget *parent) :
    QDialog(parent), ui(new Ui::SvodFileInfoDialog), entry(entry), svod(svod), type(type)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    // load the information
    ui->lblLocation->setText(QString::fromStdString(entry->filePath));
    ui->txtName->setText(QString::fromStdString(entry->name));
    ui->lblSize->setText(QString::fromStdString(ByteSizeToString(entry->size)) + " (" + QString::number(entry->size) + ")");
    ui->lblSector->setText("0x" + QString::number(entry->sector, 16).toUpper());

    DWORD sizeOnDisk = (entry->size + 0x7FF) & 0xFFFFF800;
    ui->lblSizeOnDisk->setText(QString::fromStdString(ByteSizeToString(sizeOnDisk)) + " (" + QString::number(sizeOnDisk) + " + bytes)");
    ui->lblTypeOfFile->setText(getFileType(QString::fromStdString(entry->name)));

    // load flags
    ui->chArchive->setChecked(entry->attributes & GdfxArchive);
    ui->chDevice->setChecked(entry->attributes & GdfxDevice);
    ui->chDirectory->setChecked(entry->attributes & GdfxDirectory);
    ui->chHidden->setChecked(entry->attributes & GdfxHidden);
    ui->chNormal->setChecked(entry->attributes & GdfxNormal);
    ui->chReadOnly->setChecked(entry->attributes & GdfxReadOnly);
    ui->chSystem->setChecked(entry->attributes & GdfxSystem);

    setWindowTitle(QString::fromStdString(entry->name) + " Properties");
}

SvodFileInfoDialog::~SvodFileInfoDialog()
{
    delete ui;
}

QString SvodFileInfoDialog::getFileType(QString fileName)
{
    QString extension = fileName.mid(fileName.lastIndexOf(".") + 1);
    if (extension == fileName && (type == ""))
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/DefaultFileIcon.png"));
        return "FILE";
    }

    if (type == "Image")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/ImageFileIcon.png"));
        return "Image (." + extension + ")";
    }
    else if (type == "XEX")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/XEXFileIcon.png"));
        return "Xenon Executable (." + extension + ")";
    }
    else if (type == "STFS")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/PackageFileIcon.png"));
        return "STFS Package";
    }
    else if (type == "Xdbf")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/XdbfFileIcon.png"));
        return "Xbox Database File";
    }
    else if (type == "STRB")
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/StrbFileIcon.png"));
        return "Structured Binary";
    }
    else
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/DefaultFileIcon.png"));
        return extension.toUpper() + " (." + extension + ")";
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
    entry->name = ui->txtName->text().toStdString();
    entry->attributes = 0;

    entry->attributes |= ui->chReadOnly->isChecked();
    entry->attributes |= ui->chHidden->isChecked() << 1;
    entry->attributes |= ui->chSystem->isChecked() << 2;
    entry->attributes |= ui->chDirectory->isChecked() << 4;
    entry->attributes |= ui->chArchive->isChecked() << 5;
    entry->attributes |= ui->chDevice->isChecked() << 6;
    entry->attributes |= ui->chNormal->isChecked() << 7;

    svod->WriteFileEntry(entry);
    close();
}
