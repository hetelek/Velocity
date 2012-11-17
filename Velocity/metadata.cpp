#include "metadata.h"
#include "ui_metadata.h"

AssetSubcategoryStruct subcategoryStructs[] =
{
    { "Carryable, Carryable", (AssetSubcategory)0x44c },
    { "Costume, Casual Suit", (AssetSubcategory)0x68 },
    { "Costume, Costume", (AssetSubcategory)0x69 },
    { "Costume, Formal Suit", (AssetSubcategory)0x67 },
    { "Costume, Long Dress", (AssetSubcategory)0x65 },
    { "Costume, Short Dress", (AssetSubcategory)100 },
    { "Earrings, Danglers", (AssetSubcategory)0x387 },
    { "Earrings, Largehoops", (AssetSubcategory)0x38b },
    { "Earrings, Single Dangler", (AssetSubcategory)0x386 },
    { "Earrings, Single Large Hoop", (AssetSubcategory)0x38a },
    { "Earrings, Single Small Hoop", (AssetSubcategory)0x388 },
    { "Earrings, Single Stud", (AssetSubcategory)900 },
    { "Earrings, Small Hoops", (AssetSubcategory)0x389 },
    { "Earrings, Studs", (AssetSubcategory)0x385 },
    { "Glasses, Costume", (AssetSubcategory)0x2be },
    { "Glasses, Glasses", (AssetSubcategory)700 },
    { "Glasses, Sunglasses", (AssetSubcategory)0x2bd },
    { "Gloves, Fingerless", (AssetSubcategory)600 },
    { "Gloves, Full Fingered", (AssetSubcategory)0x259 },
    { "Hat, Baseball Cap", (AssetSubcategory)0x1f6 },
    { "Hat, Beanie", (AssetSubcategory)500 },
    { "Hat, Bearskin", (AssetSubcategory)0x1fc },
    { "Hat, Brimmed", (AssetSubcategory)0x1f8 },
    { "Hat, Costume", (AssetSubcategory)0x1fb },
    { "Hat, Fez", (AssetSubcategory)0x1f9 },
    { "Hat, FlatCap", (AssetSubcategory)0x1f5 },
    { "Hat, Headwrap", (AssetSubcategory)0x1fa },
    { "Hat, Helmet", (AssetSubcategory)0x1fd },
    { "Hat, PeakCap", (AssetSubcategory)0x1f7 },
    { "Ring, Left", (AssetSubcategory)0x3e9 },
    { "Ring, Right", (AssetSubcategory)0x3e8 },
    { "Shirt, Coat", (AssetSubcategory)210 },
    { "Shirt, Hoodie", (AssetSubcategory)0xd0 },
    { "Shirt, Jacket", (AssetSubcategory)0xd1 },
    { "Shirt, Long Sleeve Shirt", (AssetSubcategory)0xce },
    { "Shirt, Long Sleeve Tee", (AssetSubcategory)0xcc },
    { "Shirt, Polo", (AssetSubcategory)0xcb },
    { "Shirt, Short Sleeve Shirt", (AssetSubcategory)0xcd },
    { "Shirt, Sports Tee", (AssetSubcategory)200 },
    { "Shirt, Sweater", (AssetSubcategory)0xcf },
    { "Shirt, Tee", (AssetSubcategory)0xc9 },
    { "Shirt, Vest", (AssetSubcategory)0xca },
    { "Shoes, Costume", (AssetSubcategory)0x197 },
    { "Shoes, Formal", (AssetSubcategory)0x193 },
    { "Shoes, Heels", (AssetSubcategory)0x191 },
    { "Shoes, High Boots", (AssetSubcategory)0x196 },
    { "Shoes, Pumps", (AssetSubcategory)0x192 },
    { "Shoes, Sandals", (AssetSubcategory)400 },
    { "Shoes, Short Boots", (AssetSubcategory)0x195 },
    { "Shoes, Trainers", (AssetSubcategory)0x194 },
    { "Trousers, Cargo", (AssetSubcategory)0x131 },
    { "Trousers, Hotpants", (AssetSubcategory)300 },
    { "Trousers, Jeans", (AssetSubcategory)0x132 },
    { "Trousers, Kilt", (AssetSubcategory)0x134 },
    { "Trousers, Leggings", (AssetSubcategory)0x12f },
    { "Trousers, Long Shorts", (AssetSubcategory)0x12e },
    { "Trousers, Long Skirt", (AssetSubcategory)0x135 },
    { "Trousers, Shorts", (AssetSubcategory)0x12d },
    { "Trousers, Short Skirt", (AssetSubcategory)0x133 },
    { "Trousers, Trousers", (AssetSubcategory)0x130 },
    { "Wristwear, Bands", (AssetSubcategory)0x322 },
    { "Wristwear, Bracelet", (AssetSubcategory)800 },
    { "Wristwear, Sweatbands", (AssetSubcategory)0x323 },
    { "Wristwear, Watch", (AssetSubcategory)0x321 }
};

OnlineResumeStateStruct resumeStates[] =
{
    { "File Headers Not Ready", (OnlineContentResumeState)0x46494C48 },
    { "New Folder", (OnlineContentResumeState)0x666F6C64 },
    { "New Folder Resume Attempt1", (OnlineContentResumeState)0x666F6C31 },
    { "New Folder Resume Attempt2", (OnlineContentResumeState)0x666F6C32 },
    { "New Folder Resume Attempt Unknown", (OnlineContentResumeState)0x666F6C3F },
    { "New Folder Resume Attempt Specific", (OnlineContentResumeState)0x666F6C40 }
};

Metadata::Metadata(QStatusBar *statusBar, StfsPackage *package, QWidget *parent) :
    QDialog(parent), ui(new Ui::Metadata), package(package), statusBar(statusBar), cmbxSubcategory(NULL), cmbxSkeletonVersion(NULL), cmbxResumeState(NULL), lastModified(NULL), offset(0)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    btnCertificate = new QPushButton(this);


    if (package->IsPEC())
    {
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    else
    {
        // set the magic
        cmbxMagic = new QComboBox(this);
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
    }

    // set the certificate button;
    btnCertificate->setText("View Certificate");
    btnCertificate->setIcon(QIcon(":/Images/certificate.png"));
    ui->tableWidget->setCellWidget(1, 0, btnCertificate);
    connect(btnCertificate, SIGNAL(clicked()), this, SLOT(btnCertificateClicked()));

    if (!package->IsPEC())
    {
        // set the license table button
        btnLicense = new QPushButton(this);
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
    }

    // set the volume descriptor button
    btnVolDesc = new QPushButton(this);
    btnVolDesc->setText("View Volume Descriptor");
    btnVolDesc->setIcon(QIcon(":/Images/descriptor.png"));
    ui->tableWidget->setCellWidget(16, 0, btnVolDesc);
    connect(btnVolDesc, SIGNAL(clicked()), this, SLOT(btnVolDescClicked()));

    if (!package->IsPEC())
    {
        // set some more simple ones
        ui->tableWidget->setItem(17, 0, new QTableWidgetItem(QString::number(package->metaData->dataFileCount)));
        ui->tableWidget->setItem(18, 0, new QTableWidgetItem(QString::number(package->metaData->dataFileCombinedSize)));

        // add special metadata
        if (package->metaData->contentType == AvatarItem)
        {
            ui->tableWidget->insertRow(19);
            ui->tableWidget->setVerticalHeaderItem(19, new QTableWidgetItem("Asset Subcategory"));

            // populate the subcategory combobox
            cmbxSubcategory = new QComboBox(this);
            for (DWORD i = 0; i < 64; i++)
            {
                cmbxSubcategory->addItem(subcategoryStructs[i].name);
                if (package->metaData->subCategory == subcategoryStructs[i].value)
                    cmbxSubcategory->setCurrentIndex(i);
            }
            ui->tableWidget->setCellWidget(19, 0, cmbxSubcategory);

            ui->tableWidget->insertRow(20);
            ui->tableWidget->setVerticalHeaderItem(20, new QTableWidgetItem("Colorizable"));
            ui->tableWidget->setItem(20, 0, new QTableWidgetItem(QString::number(package->metaData->colorizable)));

            ui->tableWidget->insertRow(21);
            ui->tableWidget->setVerticalHeaderItem(21, new QTableWidgetItem("GUID"));
            ui->tableWidget->setItem(21, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->guid, 0x10, false)));

            ui->tableWidget->insertRow(22);
            ui->tableWidget->setVerticalHeaderItem(22, new QTableWidgetItem("Skeleton Version"));

            // set the skeleton version combobox
            cmbxSkeletonVersion = new QComboBox(this);
            cmbxSkeletonVersion->addItem("Nxe");
            cmbxSkeletonVersion->addItem("Natal");
            cmbxSkeletonVersion->addItem("Both");

            cmbxSkeletonVersion->setCurrentIndex(package->metaData->skeletonVersion - 1);
            ui->tableWidget->setCellWidget(22, 0, cmbxSkeletonVersion);

            offset = 4;
        }
        else if (package->metaData->contentType == Video)
        {
            ui->tableWidget->insertRow(19);
            ui->tableWidget->setVerticalHeaderItem(19, new QTableWidgetItem("Series ID"));
            ui->tableWidget->setItem(19, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->seriesID, 0x10, false)));

            ui->tableWidget->insertRow(20);
            ui->tableWidget->setVerticalHeaderItem(20, new QTableWidgetItem("Season ID"));
            ui->tableWidget->setItem(20, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->seasonID, 0x10, false)));

            ui->tableWidget->insertRow(21);
            ui->tableWidget->setVerticalHeaderItem(21, new QTableWidgetItem("Season Number"));
            ui->tableWidget->setItem(21, 0, new QTableWidgetItem(QString::number(package->metaData->seasonNumber)));

            ui->tableWidget->insertRow(22);
            ui->tableWidget->setVerticalHeaderItem(22, new QTableWidgetItem("Episode Number"));
            ui->tableWidget->setItem(22, 0, new QTableWidgetItem(QString::number(package->metaData->episodeNumber)));

            offset = 4;
        }

        // set the device id
        ui->tableWidget->setItem(19 + offset, 0, new QTableWidgetItem(QtHelpers::ByteArrayToString(package->metaData->deviceID, 0x14, false)));

        // set the strings
        ui->tableWidget->setItem(20 + offset, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->displayName)));
        ui->tableWidget->setItem(21 + offset, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->displayDescription)));
        ui->tableWidget->setItem(22 + offset, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->publisherName)));
        ui->tableWidget->setItem(23 + offset, 0, new QTableWidgetItem(QString::fromStdWString(package->metaData->titleName)));

        // set the transfer flags button
        btnTransFlags = new QPushButton(this);
        btnTransFlags->setText("View Transfer Flags");
        btnTransFlags->setIcon(QIcon(":/Images/flag.png"));
        ui->tableWidget->setCellWidget(24 + offset, 0, btnTransFlags);
        connect(btnTransFlags, SIGNAL(clicked()), this, SLOT(btnTransFlagsClicked()));

        // set the image sizes
        ui->tableWidget->setItem(25 + offset, 0, new QTableWidgetItem(QString::number(package->metaData->thumbnailImageSize)));
        ui->tableWidget->setItem(26 + offset, 0, new QTableWidgetItem(QString::number(package->metaData->titleThumbnailImageSize)));

        switch (package->metaData->installerType)
        {
            case SystemUpdate:
            case TitleUpdate:
            {
                // installer type
                cmbxInstallerType = new QComboBox(this);
                cmbxInstallerType->addItem("System Update");
                cmbxInstallerType->addItem("Title Update");
                cmbxInstallerType->setCurrentIndex(package->metaData->installerType == TitleUpdate);

                ui->tableWidget->insertRow(27 + offset);
                ui->tableWidget->setVerticalHeaderItem(27 + offset, new QTableWidgetItem("Installer Type"));
                ui->tableWidget->setCellWidget(27 + offset, 0, cmbxInstallerType);

                // base version
                ui->tableWidget->insertRow(28 + offset);
                ui->tableWidget->setVerticalHeaderItem(28 + offset, new QTableWidgetItem("Installer Base Verison"));
                QString baseVersion = QString::number(package->metaData->installerBaseVersion.major) + "." +
                        QString::number(package->metaData->installerBaseVersion.minor) + "." +
                        QString::number(package->metaData->installerBaseVersion.build) + "." +
                        QString::number(package->metaData->installerBaseVersion.revision);
                ui->tableWidget->setItem(28 + offset, 0, new QTableWidgetItem(baseVersion));

                // version
                ui->tableWidget->insertRow(29 + offset);
                ui->tableWidget->setVerticalHeaderItem(29 + offset, new QTableWidgetItem("Installer Verison"));
                QString version = QString::number(package->metaData->installerVersion.major) + "." +
                        QString::number(package->metaData->installerVersion.minor) + "." +
                        QString::number(package->metaData->installerVersion.build) + "." +
                        QString::number(package->metaData->installerVersion.revision);
                ui->tableWidget->setItem(29 + offset, 0, new QTableWidgetItem(version));
                break;
            }
            case SystemUpdateProgressCache:
            case TitleUpdateProgressCache:
            case TitleContentProgressCache:
            {
                // set resume state combobox
                cmbxResumeState = new QComboBox(this);
                int indexSet = -1;
                for (DWORD i = 0; i < 6; i++)
                {
                    cmbxResumeState->addItem(resumeStates[i].name);
                    if (package->metaData->resumeState == resumeStates[i].value)
                    {
                        cmbxResumeState->setCurrentIndex(i);
                        indexSet = i;
                    }
                }
                if (indexSet == -1)
                {
                    cmbxResumeState->addItem("Unknown");
                    cmbxResumeState->setCurrentIndex(6);
                }

                // add to it to the list
                ui->tableWidget->insertRow(27 + offset);
                ui->tableWidget->setVerticalHeaderItem(27 + offset, new QTableWidgetItem("Online Resume State"));
                ui->tableWidget->setCellWidget(27 + offset, 0, cmbxResumeState);

                // current file index
                ui->tableWidget->insertRow(28 + offset);
                ui->tableWidget->setVerticalHeaderItem(28 + offset, new QTableWidgetItem("Current File Index"));
                ui->tableWidget->setItem(28 + offset, 0, new QTableWidgetItem(QString::number(package->metaData->currentFileIndex)));

                // current file offset
                ui->tableWidget->insertRow(29 + offset);
                ui->tableWidget->setVerticalHeaderItem(29 + offset, new QTableWidgetItem("Current File Offset"));
                ui->tableWidget->setItem(29 + offset, 0, new QTableWidgetItem(QString::number(package->metaData->currentFileOffset)));

                // current file index
                ui->tableWidget->insertRow(30 + offset);
                ui->tableWidget->setVerticalHeaderItem(30 + offset, new QTableWidgetItem("Bytes Processed"));
                ui->tableWidget->setItem(30 + offset, 0, new QTableWidgetItem(QString::number(package->metaData->bytesProcessed)));

                // last modified
                lastModified = new QDateTimeEdit(this);
                lastModified->setDateTime(QDateTime::fromTime_t(package->metaData->lastModified));

                ui->tableWidget->insertRow(31 + offset);
                ui->tableWidget->setVerticalHeaderItem(31 + offset, new QTableWidgetItem("Last Modified"));
                ui->tableWidget->setCellWidget(31 + offset, 0, lastModified);
                break;
            }
            default:
                break;
        }
    }

    statusBar->showMessage("Loaded metadata successfully", 3000);
}

void Metadata::btnCertificateClicked()
{
    CertificateDialog cert(statusBar, &package->metaData->certificate, this);
    cert.exec();
}

void Metadata::btnVolDescClicked()
{
    VolumeDescriptorDialog dialog(statusBar, &package->metaData->volumeDescriptor, this);
    dialog.exec();
}

void Metadata::btnTransFlagsClicked()
{
    TransferFlagsDialog dialog(statusBar, &package->metaData->transferFlags, this);
    dialog.exec();
}

void Metadata::btnLicenseClicked()
{
    bool unlockable = (package->metaData->contentType == AvatarItem || package->metaData->contentType == ArcadeGame || package->metaData->contentType == MarketPlaceContent);
    LicensingDataDialog dialog(statusBar, package->metaData->licenseData, unlockable, this);
    dialog.exec();
}

Metadata::~Metadata()
{
    delete ui;
}

void Metadata::on_pushButton_clicked()
{
    // verify everything
    if (!package->IsPEC())
    {
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
        if (package->metaData->contentType == Video && !QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(19, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The SeriesID value must be all digits.\n");
            return;
        }
        if (package->metaData->contentType == Video && !QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(20, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The SeasonID value must be all digits.\n");
            return;
        }
        if (package->metaData->contentType == Video && !QtHelpers::VerifyDecimalString(ui->tableWidget->item(21, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Season Number value must be all digits.\n");
            return;
        }
        if (package->metaData->contentType == Video && !QtHelpers::VerifyDecimalString(ui->tableWidget->item(22, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Episode Number value must be all digits.\n");
            return;
        }
        if (package->metaData->contentType == AvatarItem && !QtHelpers::VerifyDecimalString(ui->tableWidget->item(20, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Colorizable value must be all digits.\n");
            return;
        }
        if (package->metaData->contentType == AvatarItem && (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(21, 0)->text()) || ui->tableWidget->item(21, 0)->text().length() != 0x20))
        {
            QMessageBox::warning(this, "Invalid Value", "The GUID must be 16 hexadecimal digits.\n");
            return;
        }
        if (!QtHelpers::VerifyHexStringBuffer(ui->tableWidget->item(19 + offset, 0)->text()) || ui->tableWidget->item(19 + offset, 0)->text().length() != 40)
        {
            QMessageBox::warning(this, "Invalid Value", "The Device ID must be 40 hexadecimal digits.\n");
            return;
        }
        if (ui->tableWidget->item(20 + offset, 0)->text().length() > 0x80)
        {
            QMessageBox::warning(this, "Invalid Length", "The maximum length for the Display Name is 128.\n");
            return;
        }
        if (ui->tableWidget->item(21 + offset, 0)->text().length() > 0x80)
        {
            QMessageBox::warning(this, "Invalid Length", "The maximum length for the Display Description is 128.\n");
            return;
        }
        if (ui->tableWidget->item(22 + offset, 0)->text().length() > 0x80)
        {
            QMessageBox::warning(this, "Invalid Length", "The maximum length for the Publisher Name is 128.\n");
            return;
        }
        if (ui->tableWidget->item(23 + offset, 0)->text().length() > 0x80)
        {
            QMessageBox::warning(this, "Invalid Length", "The maximum length for the Title Name is 128.\n");
            return;
        }
        if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(25 + offset, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Thumbnail Image Size must be all digits.\n");
            return;
        }
        if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(26 + offset, 0)->text()))
        {
            QMessageBox::warning(this, "Invalid Value", "The Title Thumbnail Image Size must be all digits.\n");
            return;
        }
        if (package->metaData->installerType == TitleUpdate || package->metaData->installerType == SystemUpdate)
        {
            Version tempbv, tempv;
            if (!QtHelpers::ParseVersionString(ui->tableWidget->item(28 + offset, 0)->text(), &tempbv))
            {
                QMessageBox::warning(this, "Invalid Value", "Invalid value for the installer base version. The version should be in the following format, at or below the values provided:\n\n15.15.65535.255\n");
                return;
            }
            if (!QtHelpers::ParseVersionString(ui->tableWidget->item(29 + offset, 0)->text(), &tempv))
            {
                QMessageBox::warning(this, "Invalid Value", "Invalid value for the installer version. The version should be in the following format, at or below the values provided:\n\n15.15.65535.255\n");
                return;
            }

            package->metaData->installerBaseVersion = tempbv;
            package->metaData->installerVersion = tempv;

            package->metaData->installerType = (cmbxInstallerType->currentIndex() == 0) ? SystemUpdate : TitleUpdate;
        }
        if (package->metaData->installerType == TitleUpdateProgressCache || package->metaData->installerType == SystemUpdateProgressCache || package->metaData->installerType == TitleContentProgressCache)
        {
            if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(28 + offset, 0)->text()))
            {
                QMessageBox::warning(this, "Invalid Value", "The Current File Index must be all digits.\n");
                return;
            }
            if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(29 + offset, 0)->text()))
            {
                QMessageBox::warning(this, "Invalid Value", "The Current File Offset must be all digits.\n");
                return;
            }
            if (!QtHelpers::VerifyDecimalString(ui->tableWidget->item(30 + offset, 0)->text()))
            {
                QMessageBox::warning(this, "Invalid Value", "The Bytes Processed must be all digits.\n");
                return;
            }
        }

        // update all the values
        DWORD magics[3] = { CON, LIVE, PIRS };
        package->metaData->magic = (Magic)magics[cmbxMagic->currentIndex()];
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

        if (package->metaData->contentType == AvatarItem)
        {
            package->metaData->subCategory = subcategoryStructs[cmbxSubcategory->currentIndex()].value;
            package->metaData->colorizable = ui->tableWidget->item(20, 0)->text().toULong();
            QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(21, 0)->text(), package->metaData->guid, 0x10);
            package->metaData->skeletonVersion = (SkeletonVersion)(cmbxSkeletonVersion->currentIndex() + 1);
        }
        else if (package->metaData->contentType == Video)
        {
            QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(19, 0)->text(), package->metaData->seriesID, 0x10);
            QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(20, 0)->text(), package->metaData->seasonID, 0x10);
            package->metaData->seasonNumber = ui->tableWidget->item(21, 0)->text().toUShort();
            package->metaData->episodeNumber = ui->tableWidget->item(22, 0)->text().toUShort();
        }

        QtHelpers::ParseHexStringBuffer(ui->tableWidget->item(19 + offset, 0)->text(), package->metaData->deviceID, 0x14);
        package->metaData->displayName = ui->tableWidget->item(20 + offset, 0)->text().toStdWString();
        package->metaData->displayDescription = ui->tableWidget->item(21 + offset, 0)->text().toStdWString();
        package->metaData->publisherName = ui->tableWidget->item(22 + offset, 0)->text().toStdWString();
        package->metaData->titleName = ui->tableWidget->item(23 + offset, 0)->text().toStdWString();
        package->metaData->thumbnailImageSize = ui->tableWidget->item(25 + offset, 0)->text().toULong();
        package->metaData->titleThumbnailImageSize = ui->tableWidget->item(26 + offset, 0)->text().toULong();

        if (package->metaData->installerType == TitleUpdateProgressCache || package->metaData->installerType == SystemUpdateProgressCache || package->metaData->installerType == TitleContentProgressCache)
        {
            if (cmbxResumeState->currentIndex() != 6)
                package->metaData->resumeState = resumeStates[cmbxResumeState->currentIndex()].value;
            package->metaData->currentFileIndex = ui->tableWidget->item(28 + offset, 0)->text().toULong();
            package->metaData->currentFileOffset = ui->tableWidget->item(29 + offset, 0)->text().toULongLong();
            package->metaData->bytesProcessed = ui->tableWidget->item(30 + offset, 0)->text().toULongLong();
            package->metaData->lastModified = lastModified->dateTime().toTime_t();
        }

    }

    try
    {
        package->metaData->WriteMetaData();
        statusBar->showMessage("Saved metadata successfully", 3000);
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
