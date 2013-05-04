#include "flashdriveconfigdatadialog.h"
#include "ui_flashdriveconfigdatadialog.h"

FlashDriveConfigDataDialog::FlashDriveConfigDataDialog(FlashDriveConfigurationData *configData, QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::FlashDriveConfigDataDialog), statusBar(statusBar), configData(configData)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->lblDeviceID->setText(QtHelpers::ByteArrayToString(configData->deviceID, 0x14, false));
    ui->lblDeviceSize->setText(QString::fromStdString(ByteSizeToString(configData->deviceLength)));
    ui->lblReadSpeed->setText(QString::fromStdString(ByteSizeToString(configData->readSpeed)) + "/second");
    ui->lblWriteSpeed->setText(QString::fromStdString(ByteSizeToString(configData->WriteSpeed)) + "/second");
    ui->lblSecuritySize->setText("0x" + QString::number(configData->securityLength, 16));

    if (configData->securityLength == 0x228)
    {
        ui->btnCertificate->setVisible(true);
        ui->txtSignature->setText(QtHelpers::ByteArrayToString(configData->certificate.signature, 0x80, true));
    }
    else
    {
        ui->btnCertificate->setVisible(false);
        ui->txtSignature->setText(QtHelpers::ByteArrayToString(configData->deviceSignature, 0x100, true));
    }
}

FlashDriveConfigDataDialog::~FlashDriveConfigDataDialog()
{
    delete ui;
}

void FlashDriveConfigDataDialog::on_btnCertificate_clicked()
{
    CertificateDialog dialog(statusBar, &configData->certificate, this, true);
    dialog.exec();
}
