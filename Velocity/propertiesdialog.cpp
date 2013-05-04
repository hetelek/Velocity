#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"

PropertiesDialog::PropertiesDialog(StfsFileEntry *entry, QString location, bool *changed, QIcon icon, bool hasChildren, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesDialog),
    changed(changed),
    entry(entry)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);
    setWindowTitle(QString::fromStdString(entry->name) + " Properties");

    bool folder = (entry->flags & 2);
    location = location.insert(0, '\\');
    if (folder)
        location = location.mid(0, location.lastIndexOf('\\'));

    if (location.isEmpty())
        location = "\\";

    ui->lblIcon->setPixmap(icon.pixmap(24, 24));

    // if it's a file, remove the filename
    if (!folder)
    {
        int index = location.lastIndexOf('\\');
        if (index != -1)
            location = location.mid(0, index);
        else
            location = "";
    }

    ui->leName->setText(QString::fromStdString(entry->name));

    // get the extension
    size_t index = entry->name.rfind('.');
    QString exten;
    if (entry->flags & 2)
        exten = "File folder";
    else if (index != string::npos)
        exten = QString::fromStdString(entry->name.substr(index)).toLower();
    else
        exten = "File";

    // get the friendly name of the extension (if any)
    QString final;
    if (exten != "File" && exten != "File folder")
    {
        QString friendlyName;
        if (exten == ".gpd")
            friendlyName = "Game Progress Data";
        else if (exten == ".png" || exten == ".jpg" || exten == ".jpeg")
            friendlyName = "Image";
        else if (exten == ".fit")
            friendlyName = "Fitness File";
        else
            friendlyName = exten.mid(1).toUpper() + " File";
        final = friendlyName + " (" + exten + ")";
    }
    else
        final = exten;

    ui->lblTypeOfFile->setText(final);
    ui->lblLocation->setText(location);
    ui->lblSize->setText(QString::fromStdString(ByteSizeToString(entry->fileSize)) + " (" + QString::number(entry->fileSize) + " bytes)");

    int sizeOnDisk = (entry->fileSize + 0xFFF) & 0xFFFFF000;
    ui->lblSizeOnDisk->setText(QString::fromStdString(ByteSizeToString(sizeOnDisk)) + " (" + QString::number(sizeOnDisk) + " bytes)");

    // get the MSTime from
    MSTime createdtime = DWORDToMSTime(entry->createdTimeStamp);
    MSTime accesstime = DWORDToMSTime(entry->accessTimeStamp);
    QDateTime dateTime;

    QDate date;
    QTime time;

    date.setDate(createdtime.year, createdtime.month, createdtime.monthDay);
    time.setHMS(createdtime.hours, createdtime.minutes, createdtime.seconds);

    dateTime.setDate(date);
    dateTime.setTime(time);

    QString dateStr = dateTime.toString("dddd, MMMM dd, yyyy, h:mm:ss AP");
    if (dateStr.isEmpty())
        dateStr = "<i>N/A</i>";
    ui->lblCreated->setText(dateStr);

    date.setDate(accesstime.year, accesstime.month, accesstime.monthDay);
    time.setHMS(accesstime.hours, accesstime.minutes, accesstime.seconds);

    dateTime.setDate(date);
    dateTime.setTime(time);

    dateStr = dateTime.toString("dddd, MMMM dd, yyyy, h:mm:ss AP");
    if (dateStr.isEmpty())
        dateStr = "<i>N/A</i>";
    ui->lblAccessed->setText(dateStr);

    ui->cbConsecutive->setCheckState((Qt::CheckState)((entry->flags & 1) << 1));
    ui->cbFolder->setCheckState((Qt::CheckState)(folder << 1));

    if (hasChildren)
        ui->cbFolder->setEnabled(false);

    *changed = false;
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}

void PropertiesDialog::on_btnCancel_clicked()
{
    close();
}

void PropertiesDialog::on_leName_textChanged(const QString &arg1)
{
    if (arg1.toStdString() != entry->name && !ui->btnApply->isEnabled())
        ui->btnApply->setEnabled(true);
}

void PropertiesDialog::on_cbConsecutive_toggled(bool checked)
{
    if (checked != (entry->flags & 1) && !ui->btnApply->isEnabled())
        ui->btnApply->setEnabled(true);
}

void PropertiesDialog::on_cbFolder_toggled(bool checked)
{
    if (checked != (entry->flags & 2) >> 1 && !ui->btnApply->isEnabled())
        ui->btnApply->setEnabled(true);
}

void PropertiesDialog::updateEntry()
{
    entry->name = ui->leName->text().toStdString();

    int consecState = ui->cbConsecutive->checkState();
    int folderState = ui->cbFolder->checkState();

    entry->flags = (consecState >> 1) | folderState;

    *changed = true;
}

void PropertiesDialog::on_btnOK_clicked()
{
    updateEntry();
    close();
}

void PropertiesDialog::on_btnApply_clicked()
{
    updateEntry();
    ui->btnApply->setEnabled(false);
}
