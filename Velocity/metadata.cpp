#include "metadata.h"
#include "ui_metadata.h"

Metadata::Metadata(StfsPackage *package, QWidget *parent) : QDialog(parent), ui(new Ui::Metadata), package(package)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    btnCertificate = new QPushButton;

    // set the magic
    cmbxMagic = new QComboBox();
    cmbxMagic->addItem("CON ");
    cmbxMagic->addItem("LIVE");
    cmbxMagic->addItem("PIRS");

    ui->tableWidget->setCellWidget(0, 0, cmbxMagic);
    switch (package->metaData->magic)
    {
        case CON:
            cmbxMagic->setCurrentIndex(0);
            break;
        case LIVE:
            cmbxMagic->setCurrentIndex(1);
            btnCertificate->setEnabled(false);
            break;
        case PIRS:
            cmbxMagic->setCurrentIndex(2);
            btnCertificate->setEnabled(false);
            break;
    }

    // set the certificate button;
    btnCertificate->setText("View Certificate");
    btnCertificate->setIcon(QIcon(":/Images/certificate.png"));
    ui->tableWidget->setCellWidget(1, 0, btnCertificate);
    connect(btnCertificate, SIGNAL(clicked()), this, SLOT(btnCertificateClicked()));

    // set the license table button
    btnLicense = new QPushButton();
    btnLicense->setText("View License Table");
    btnLicense->setIcon(QIcon(":/Images/license.png"));
    ui->tableWidget->setCellWidget(2, 0, btnLicense);
    connect(btnLicense, SIGNAL(clicked()), this, SLOT(btnLicenseClicked()));

    // set the header hash
    ui->tableWidget->setItem(3, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->headerHash, 0x14, false)));

    // set some other values
    ui->tableWidget->setItem(4, 0, new QTableWidgetItem("0x" + QString::number(package->metaData->headerSize, 16).toUpper()));
    ui->tableWidget->setItem(5, 0, new QTableWidgetItem("0x" + QString::number(package->metaData->mediaID, 16).toUpper()));
    ui->tableWidget->setItem(6, 0, new QTableWidgetItem(QString::number(package->metaData->version)));
    ui->tableWidget->setItem(7, 0, new QTableWidgetItem(QString::number(package->metaData->baseVersion)));
    ui->tableWidget->setItem(8, 0, new QTableWidgetItem("0x" + QString::number(package->metaData->titleID, 16).toUpper()));
    ui->tableWidget->setItem(9, 0, new QTableWidgetItem(QString::number(package->metaData->platform)));
    ui->tableWidget->setItem(10, 0, new QTableWidgetItem(QString::number(package->metaData->executableType)));
    ui->tableWidget->setItem(11, 0, new QTableWidgetItem(QString::number(package->metaData->discNumber)));
    ui->tableWidget->setItem(12, 0, new QTableWidgetItem(QString::number(package->metaData->discInSet)));
    ui->tableWidget->setItem(13, 0, new QTableWidgetItem("0x" + QString::number(package->metaData->savegameID, 16).toUpper()));

    // set the console id
    ui->tableWidget->setItem(14, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->consoleID, 5, false)));

    // set the profile id
    ui->tableWidget->setItem(15, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->profileID, 8, false)));

    // set the volume descriptor button
    btnVolDesc = new QPushButton();
    btnVolDesc->setText("View Volume Descriptor");
    btnVolDesc->setIcon(QIcon(":/Images/descriptor.png"));
    ui->tableWidget->setCellWidget(16, 0, btnVolDesc);
    connect(btnVolDesc, SIGNAL(clicked()), this, SLOT(btnVolDescClicked()));

    // set some more simple ones
    ui->tableWidget->setItem(17, 0, new QTableWidgetItem(QString::number(package->metaData->dataFileCount)));
    ui->tableWidget->setItem(18, 0, new QTableWidgetItem(QString::number(package->metaData->dataFileCombinedSize)));

    // set the series id
    ui->tableWidget->setItem(19, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->seriesID, 16, false)));

    // set the season id
    ui->tableWidget->setItem(20, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->seasonID, 16, false)));

    // set some more simple ones
    ui->tableWidget->setItem(21, 0, new QTableWidgetItem(QString::number(package->metaData->seasonNumber)));
    ui->tableWidget->setItem(22, 0, new QTableWidgetItem(QString::number(package->metaData->episodeNumber)));

    // set the device id
    ui->tableWidget->setItem(23, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->deviceID, 0x14, false)));

    // set the strings
    ui->tableWidget->setItem(24, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->displayName)));
    ui->tableWidget->setItem(25, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->displayDescription)));
    ui->tableWidget->setItem(26, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->publisherName)));
    ui->tableWidget->setItem(27, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->titleName)));

    // set the transfer flags button
    btnTransFlags = new QPushButton();
    btnTransFlags->setText("View Transfer Flags");
    btnTransFlags->setIcon(QIcon(":/Images/flag.png"));
    ui->tableWidget->setCellWidget(28, 0, btnTransFlags);
    connect(btnTransFlags, SIGNAL(clicked()), this, SLOT(btnTransFlagsClicked()));

    // set the image sizes
    ui->tableWidget->setItem(29, 0, new QTableWidgetItem(QString::number(package->metaData->thumbnailImageSize)));
    ui->tableWidget->setItem(30, 0, new QTableWidgetItem(QString::number(package->metaData->titleThumbnailImageSize)));
}

void Metadata::btnCertificateClicked()
{
    CertificateDialog cert(&package->metaData->certificate, this);
    cert.exec();
}

void Metadata::btnVolDescClicked()
{
    VolumeDescriptorDialog dialog(&package->metaData->volumeDescriptor, this);
    dialog.exec();
}

void Metadata::btnTransFlagsClicked()
{
    TransferFlagsDialog dialog(&package->metaData->transferFlags, this);
    dialog.exec();
}

void Metadata::btnLicenseClicked()
{
    LicensingDataDialog dialog(package->metaData->licenseData, this);
    dialog.exec();
}

Metadata::~Metadata()
{
    delete cmbxMagic;
    delete ui;
}

void Metadata::on_pushButton_clicked()
{
    // verify everything
    if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(3, 0)->text()) || ui->tableWidget->item(3, 0)->text().length() != 40)
    {
        QMessageBox::warning(this, "Invalid Value", "The ContentID/Header Hash must be 40 hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(4, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Header Size must be all hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(5, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The MediaID must be all hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(6, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Version must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(7, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Base Version must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(8, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The TitleID must be all hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(9, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Platform must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(10, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Executable Type must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(11, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Disc Number must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(12, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Disc In Set must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexString(ui->tableWidget->item(13, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Save Game ID must be all hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(14, 0)->text()) || ui->tableWidget->item(14, 0)->text().length() != 10)
    {
        QMessageBox::warning(this, "Invalid Value", "The Console ID must be 10 hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(15, 0)->text()) || ui->tableWidget->item(15, 0)->text().length() != 16)
    {
        QMessageBox::warning(this, "Invalid Value", "The Profile ID must be 16 hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(17, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Data File Count must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(18, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Data File Combined Size must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(19, 0)->text()) || ui->tableWidget->item(19, 0)->text().length() != 0x20)
    {
        QMessageBox::warning(this, "Invalid Value", "The Series ID must be 32 hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(20, 0)->text()) || ui->tableWidget->item(20, 0)->text().length() != 0x20)
    {
        QMessageBox::warning(this, "Invalid Value", "The Season ID must be 32 hexadecimal digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(21, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Season Number must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(22, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Episode Number must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(23, 0)->text()) || ui->tableWidget->item(23, 0)->text().length() != 40)
    {
        QMessageBox::warning(this, "Invalid Value", "The Device ID must be 40 hexadecimal digits.\n");
        return;
    }
    if (ui->tableWidget->item(24, 0)->text().length() > 0x80)
    {
        QMessageBox::warning(this, "Invalid Length", "The maximum length for the Display Name is 128.\n");
        return;
    }
    if (ui->tableWidget->item(25, 0)->text().length() > 0x80)
    {
        QMessageBox::warning(this, "Invalid Length", "The maximum length for the Display Description is 128.\n");
        return;
    }
    if (ui->tableWidget->item(26, 0)->text().length() > 0x80)
    {
        QMessageBox::warning(this, "Invalid Length", "The maximum length for the Publisher Name is 128.\n");
        return;
    }
    if (ui->tableWidget->item(27, 0)->text().length() > 0x80)
    {
        QMessageBox::warning(this, "Invalid Length", "The maximum length for the Title Name is 128.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(29, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Thumbnail Image Size must be all digits.\n");
        return;
    }
    if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(30, 0)->text()))
    {
        QMessageBox::warning(this, "Invalid Value", "The Title Thumbnail Image Size must be all digits.\n");
        return;
    }

    // update all the values
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(3, 0)->text(), package->metaData->headerHash, 0x14);
    package->metaData->headerSize = QtHelpers::ParseHexString(ui->tableWidget->item(4, 0)->text());
    package->metaData->mediaID = QtHelpers::ParseHexString(ui->tableWidget->item(5, 0)->text());
    package->metaData->version = ui->tableWidget->item(6, 0)->text().toULong();
    package->metaData->baseVersion = ui->tableWidget->item(7, 0)->text().toULong();
    package->metaData->titleID = QtHelpers::ParseHexString(ui->tableWidget->item(8, 0)->text());
    package->metaData->platform = ui->tableWidget->item(9, 0)->text().toULong();
    package->metaData->executableType = ui->tableWidget->item(10, 0)->text().toULong();
    package->metaData->discNumber = ui->tableWidget->item(11, 0)->text().toULong();
    package->metaData->discInSet = ui->tableWidget->item(12, 0)->text().toULong();
    package->metaData->savegameID = QtHelpers::ParseHexString(ui->tableWidget->item(13, 0)->text());
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(14, 0)->text(), package->metaData->consoleID, 5);
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(15, 0)->text(), package->metaData->profileID, 8);
    package->metaData->dataFileCount = ui->tableWidget->item(17, 0)->text().toULong();
    package->metaData->dataFileCombinedSize = ui->tableWidget->item(18, 0)->text().toULongLong();
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(19, 0)->text(), package->metaData->seriesID, 0x10);
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(20, 0)->text(), package->metaData->seasonID, 0x10);
    package->metaData->seasonNumber = ui->tableWidget->item(21, 0)->text().toUShort();
    package->metaData->episodeNumber = ui->tableWidget->item(22, 0)->text().toUShort();
    QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(23, 0)->text(), package->metaData->deviceID, 0x14);
    package->metaData->displayName = ui->tableWidget->item(24, 0)->text().toStdWString();
    package->metaData->displayDescription = ui->tableWidget->item(25, 0)->text().toStdWString();
    package->metaData->publisherName = ui->tableWidget->item(26, 0)->text().toStdWString();
    package->metaData->titleName = ui->tableWidget->item(27, 0)->text().toStdWString();
    package->metaData->thumbnailImageSize = ui->tableWidget->item(29, 0)->text().toULong();
    package->metaData->titleThumbnailImageSize = ui->tableWidget->item(30, 0)->text().toULong();

    try
    {
        package->metaData->WriteMetaData();
    }
    catch(string error)
    {
        QMessageBox::critical(this, "Error", "Failed to save metadata.\n\n" + QString::fromStdString(error));
    }

    this->close();
}

void Metadata::on_pushButton_2_clicked()
{
    this->close();
}
