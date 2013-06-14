#include "securitysectordialog.h"
#include "ui_securitysectordialog.h"

SecuritySectorDialog::SecuritySectorDialog(FatxDrive *drive, QWidget *parent) :
    QDialog(parent), ui(new Ui::SecuritySectorDialog), drive(drive)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->lblFirmwareRevision->setText(QString::fromStdString(drive->securityBlob.firmwareRevision).trimmed());
    ui->lblModelNumber->setText(QString::fromStdString(drive->securityBlob.modelNumber).trimmed());
    ui->lblSerialNumber->setText(QString::fromStdString(drive->securityBlob.serialNumber).trimmed());
    ui->lblSectors->setText("0x" + QString::number(drive->securityBlob.userAddressableSectors, 16).toUpper());
}

SecuritySectorDialog::~SecuritySectorDialog()
{
    delete ui;
}

void SecuritySectorDialog::on_btnExtractSecuritySector_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to save the security blob...", QtHelpers::DesktopLocation() + "/Security Blob.bin");
    if (savePath == "")
        return;

    try
    {
        drive->ExtractSecurityBlob(savePath.toStdString());
        QMessageBox::information(this, "Success", "Successfully saved the security blob.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while extracting the security blob.\n\n" + QString::fromStdString(error));
    }
}

void SecuritySectorDialog::on_btnReplaceSecuritySector_clicked()
{
    QString openPath = QFileDialog::getOpenFileName(this, "Choose a security blob...", QtHelpers::DesktopLocation() + "/Security Blob.bin");
    if (openPath == "")
        return;

    try
    {
        drive->ReplaceSecurityBlob(openPath.toStdString());
        QMessageBox::information(this, "Success", "Successfully replaced the security blob.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while replacing the security blob.\n\n" + QString::fromStdString(error));
    }
}
