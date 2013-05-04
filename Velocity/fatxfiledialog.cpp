#include "fatxfiledialog.h"
#include "ui_fatxfiledialog.h"

FatxFileDialog::FatxFileDialog(FatxDrive *drive, FatxFileEntry *entry, DWORD clusterSize, QString type, QWidget *parent) :
    QDialog(parent), ui(new Ui::FatxFileDialog), entry(entry), type(type), drive(drive)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->txtName->setText(QString::fromStdString(entry->name));
    ui->lblCluster->setText("0x" + QString::number(entry->startingCluster, 16).toUpper());
    ui->lblCreated->setText(msTimeToString(entry->creationDate));
    ui->lblModified->setText(msTimeToString(entry->lastWriteDate));
    ui->lblAccessed->setText(msTimeToString(entry->lastAccessDate));
    ui->lblTypeOfFile->setText(getFileType(QString::fromStdString(entry->name)));
    ui->lblLocation->setText(QString::fromStdString(entry->path));

    if ((entry->fileAttributes & FatxDirectory) == 0)
    {
        ui->lblSize->setText(QString::fromStdString(ByteSizeToString(entry->fileSize)));

        UINT64 sizeOnDisk = (entry->fileSize + (clusterSize - 1)) & ~(clusterSize - 1);
        ui->lblSizeOnDisk->setText(QString::fromStdString(ByteSizeToString(sizeOnDisk)));
    }
    else
    {
        // TODO: calcuate the size of directories
        ui->lblSize->setText("N/A");
        ui->lblSizeOnDisk->setText("N/A");
    }

    ui->chArchive->setChecked(entry->fileAttributes & FatxArchive);
    ui->chDevice->setChecked(entry->fileAttributes & FatxDevice);
    ui->chDirectory->setChecked(entry->fileAttributes & FatxDirectory);
    ui->chHidden->setChecked(entry->fileAttributes & FatxHidden);
    ui->chNormal->setChecked(entry->fileAttributes & FatxNormal);
    ui->chReadOnly->setChecked(entry->fileAttributes & FatxReadOnly);
    ui->chSystem->setChecked(entry->fileAttributes & FatxSystem);

    setWindowTitle(QString::fromStdString(entry->name) + " Properties");
}

FatxFileDialog::~FatxFileDialog()
{
    delete ui;
}

QString FatxFileDialog::msTimeToString(DWORD time)
{
    MSTime msTime = DWORDToMSTime(entry->creationDate);

    QDateTime dateTime;
    dateTime.setDate(QDate(msTime.year, msTime.month, msTime.monthDay));
    dateTime.setTime(QTime(msTime.hours, msTime.minutes, msTime.seconds));

    return dateTime.toString();
}

QString FatxFileDialog::getFileType(QString fileName)
{
    QString extension = fileName.mid(fileName.lastIndexOf(".") + 1);
    if (entry->fileAttributes & FatxDirectory)
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/FolderFileIcon.png"));
        return "Directory";
    }
    else if (extension == fileName && (type == ""))
    {
        ui->imgIcon->setPixmap(QPixmap(":/Images/DefaultFileIcon.png"));
        return "FILE";
    }
    else if (type == "Image")
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

void FatxFileDialog::WriteEntryBack()
{
    entry->name = ui->txtName->text().toStdString();
    entry->fileAttributes = 0;

    if (ui->chArchive->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxArchive;
    if (ui->chDevice->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxDevice;
    if (ui->chDirectory->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxDirectory;
    if (ui->chHidden->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxHidden;
    if (ui->chNormal->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxNormal;
    if (ui->chReadOnly->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxReadOnly;
    if (ui->chSystem->checkState() == Qt::Checked)
        entry->fileAttributes |= FatxSystem;

    FatxIO io = drive->GetFatxIO(entry);

    io.entry = entry;
    io.WriteEntryToDisk();

    io.Close();
}

void FatxFileDialog::on_btnOK_clicked()
{
    WriteEntryBack();
    close();
}

void FatxFileDialog::on_btnCancel_clicked()
{
    close();
}
