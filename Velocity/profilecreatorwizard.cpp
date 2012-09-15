#include "profilecreatorwizard.h"
#include "ui_profilecreatorwizard.h"

ProfileCreatorWizard::ProfileCreatorWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ProfileCreatorWizard)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));

    // generate profile ID
    profileID = 0xE000000000000000;
    srand(time(NULL));
    profileID |= ((UINT64)rand() + ((UINT64)rand() << 16) + ((UINT64)rand() << 32));

    ui->lblSavePath->setText(QtHelpers::DesktopLocation() + "\\" + QString::number(profileID, 16).toUpper());

    ui->listWidget->item(0)->setSelected(true);
    ui->listWidget->setCurrentItem(ui->listWidget->item(0));
}

ProfileCreatorWizard::~ProfileCreatorWizard()
{
    delete ui;
}

void ProfileCreatorWizard::onCurrentIdChanged(int id)
{
    switch (id)
    {
        case 2:
            button(QWizard::NextButton)->setEnabled(ui->txtGamertag->text() != "");
            break;
    }
}

void ProfileCreatorWizard::onFinished(int status)
{
    if (status == 0)
        return;
    try
    {
        // create a new file
        FileIO io(ui->lblSavePath->text().toStdString(), true);

        // null out the first 0xD000 bytes
        BYTE tempBuffer[0x1000] = {0};
        for (DWORD i = 0; i < 0xD; i++)
            io.write(tempBuffer, 0x1000);
        io.setPosition(0xA014);
        io.write((DWORD)0x80FFFFFF);

        // set up the metadata for the profile
        StfsMetaData metadata(&io, MetadataDontFreeThumbnails | MetadataSkipRead);
        metadata.magic = CON;
        metadata.certificate.ownerConsoleType = (ui->cmbxType->currentIndex() == 0) ? Retail : DevKit;
        metadata.certificate.consoleTypeFlags = 0;

        memset(metadata.licenseData, 0, sizeof(LicenseEntry) * 0x10);
        metadata.licenseData[0].type = Unrestricted;
        metadata.licenseData[0].data = 0xFFFFFFFFFFFF;

        metadata.headerSize = 0x971A;
        metadata.contentType = Profile;
        metadata.metaDataVersion = 2;
        metadata.contentSize = 0;
        metadata.mediaID = 0;
        metadata.version = 0;
        metadata.baseVersion = 0;
        metadata.titleID = 0xFFFE07D1;
        metadata.platform = 0;
        metadata.executableType = 0;
        metadata.discNumber = 0;
        metadata.discInSet = 0;
        metadata.savegameID = 0;
        memset(metadata.consoleID, 0, 5);

        // set the profile id
        UINT64 temp = profileID;
        for (DWORD i = 0; i < 8; i++)
            ((BYTE*)&temp)[i] = ((BYTE*)&profileID)[7 - i];
        memcpy(metadata.profileID, &temp, 8);

        // volume descriptor
        metadata.volumeDescriptor.size = 0x24;
        metadata.volumeDescriptor.blockSeperation = 0;
        metadata.volumeDescriptor.fileTableBlockCount = 1;
        metadata.volumeDescriptor.fileTableBlockNum = 0;
        metadata.volumeDescriptor.allocatedBlockCount = 1;
        metadata.volumeDescriptor.unallocatedBlockCount = 0;

        metadata.dataFileCount = 0;
        metadata.dataFileCombinedSize = 0;
        metadata.seasonNumber = 0;
        metadata.episodeNumber = 0;
        memset(metadata.seasonID, 0, 0x10);
        memset(metadata.seriesID, 0, 0x10);
        memset(metadata.deviceID, 0, 0x14);
        std::wstring s = QtHelpers::ByteArrayToString((BYTE*)&temp, 8, false).toStdWString();
        metadata.displayName = s;
        metadata.displayDescription = L"";
        metadata.publisherName = L"";
        metadata.titleName = L"";
        metadata.transferFlags = 0x40;

        // set gamerpicture
        QByteArray ba1;
        QBuffer buffer1(&ba1);
        buffer1.open(QIODevice::WriteOnly);
        ui->listWidget->currentItem()->icon().pixmap(64, 64).save(&buffer1, "PNG");
        metadata.thumbnailImage = ba1.data();
        metadata.thumbnailImageSize = ba1.length();

        // set title thumbnail image
        QByteArray ba2;
        QBuffer buffer2(&ba2);
        buffer2.open(QIODevice::WriteOnly);
        QPixmap(":/Images/defaultTitleImage.png").save(&buffer2, "PNG");
        metadata.titleThumbnailImage = ba2.data();
        metadata.titleThumbnailImageSize = ba2.length();

        metadata.WriteMetaData();
        io.close();

        StfsPackage newProfile(ui->lblSavePath->text().toStdString(), false);

        // create the account file
        QString accountTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

        FileIO accountIo(accountTempPath.toStdString(), true);
        accountIo.write(tempBuffer, 380);
        accountIo.close();

        // write the gamertag, and encrypt the file
        Account account(accountTempPath.toStdString(), false, metadata.certificate.ownerConsoleType);
        account.SetGamertag(ui->txtGamertag->text().toStdWString());
        account.Save(metadata.certificate.ownerConsoleType);

        // add the account file to the package
        newProfile.InjectFile(accountTempPath.toStdString(), "Account");

        // make a temporary copy of the dashboard gpd
        QString dashGPDTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        QFile::copy("C:\\Users\\Adam\\Desktop\\FFFE07D1.gpd", dashGPDTempPath);

        // parse the GPD
        DashboardGPD dashGPD(dashGPDTempPath.toStdString());

        // change the gamerpicture key
        wstring picKey = L"fffe07d10002000" + QString::number(ui->listWidget->currentIndex().row()).toStdWString() + L"0001000" + QString::number(ui->listWidget->currentIndex().row()).toStdWString();
        dashGPD.gamerPictureKey.str = &picKey;
        dashGPD.WriteSettingEntry(dashGPD.gamerPictureKey);

        // if the avatar type is female, then we must change the avatar info setting
        if (ui->rdiFemale->isChecked())
        {
            // read in the setting
            FileIO io("C:\\Users\\Adam\\Desktop\\femaleAvatar.bin");
            BYTE settingBuffer[0x3E8];
            io.readBytes(settingBuffer, 0x3E8);

            dashGPD.avatarInformation.binaryData.data = settingBuffer;
            dashGPD.WriteSettingEntry(dashGPD.avatarInformation);
        }

        dashGPD.Close();

        // inject the dash gpd into the profile
        newProfile.InjectFile(dashGPDTempPath.toStdString(), "FFFE07D1.gpd");

        // create/inject the 64x64 image
        QString img64Path = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        ui->listWidget->currentItem()->icon().pixmap(64, 64).save(img64Path, "PNG");
        newProfile.InjectFile(img64Path.toStdString(), "tile_64.png");

        // create/inject the 32x32 image
        QString img32Path = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        ui->listWidget->currentItem()->icon().pixmap(32, 32).save(img32Path, "PNG");
        newProfile.InjectFile(img32Path.toStdString(), "tile_32.png");

        newProfile.Rehash();
        string path = QtHelpers::GetKVPath(newProfile.metaData->certificate.ownerConsoleType, this);

        if (path != "")
            newProfile.Resign(path);

        // delete the temp files
        QFile::remove(accountTempPath);
        QFile::remove(dashGPDTempPath);
        QFile::remove(img64Path);
        QFile::remove(img32Path);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occured while creating the profile.\n\n" + QString::fromStdString(error));
    }
}

bool ProfileCreatorWizard::verifyGamertag(QString gamertag)
{
    if (gamertag.length() == 0 || gamertag.length() > 15 || !gamertag.at(0).isLetter())
        return false;

    for (DWORD i = 1; i < gamertag.length(); i++)
        if (!gamertag.at(i).isLetterOrNumber())
            return false;
    return true;
}

void ProfileCreatorWizard::on_txtGamertag_textChanged(const QString &arg1)
{
    if (!verifyGamertag(ui->txtGamertag->text()))
    {
        ui->txtGamertag->setStyleSheet("color: rgb(255, 1, 1);");
        button(QWizard::NextButton)->setEnabled(false);
    }
    else
    {
        ui->txtGamertag->setStyleSheet("");
        button(QWizard::NextButton)->setEnabled(true);
    }
}
