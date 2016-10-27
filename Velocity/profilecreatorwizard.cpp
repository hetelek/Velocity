#include "profilecreatorwizard.h"
#include "ui_profilecreatorwizard.h"

ProfileCreatorWizard::ProfileCreatorWizard(QStatusBar *statusBar, QWidget *parent) :
    QWizard(parent), ui(new Ui::ProfileCreatorWizard), statusBar(statusBar), consoleType(Retail)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

#ifdef __APPLE__
    ui->label_3->setText("<font size=3>" + ui->label_3->text() + "</font>");
    ui->label_5->setText("<font size=3>" + ui->label_5->text() + "</font>");
    ui->label_7->setText("<font size=3>" + ui->label_7->text() + "</font>");
    ui->lblSavePath->setText("<font size=3>" + ui->lblSavePath->text() + "</font>");
#endif

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));

    // generate profile ID
    profileID = 0xE000000000000000;
    srand(time(NULL));
    profileID |= ((UINT64)rand() & 0xFFFF) + (((UINT64)rand() & 0xFFFF) + (((UINT64)rand() & 0xFFFF) << 32));

    ui->lblSavePath->setText(QtHelpers::DesktopLocation().replace("\\", "/") + "/" + QString::number(profileID, 16).toUpper());

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
        StfsPackage newProfile(ui->lblSavePath->text().toStdString(), StfsPackageCreate);

        // set up the metadata for the profile
        newProfile.metaData->magic = CON;
        newProfile.metaData->certificate.ownerConsoleType = consoleType;

        newProfile.metaData->contentType = Profile;
        newProfile.metaData->metaDataVersion = 2;
        newProfile.metaData->titleID = 0xFFFE07D1;

        // set the profile id
        UINT64 temp = profileID;
        for (DWORD i = 0; i < 8; i++)
            ((BYTE*)&temp)[i] = ((BYTE*)&profileID)[7 - i];
        memcpy(newProfile.metaData->profileID, &temp, 8);

        std::wstring s = QtHelpers::ByteArrayToString((BYTE*)&temp, 8, false).toStdWString();
        newProfile.metaData->displayName = s;
        newProfile.metaData->transferFlags = 0x40;

        // set gamerpicture
        QByteArray ba1;
        QBuffer buffer1(&ba1);
        buffer1.open(QIODevice::WriteOnly);
        ui->listWidget->currentItem()->icon().pixmap(64, 64).save(&buffer1, "PNG");
        newProfile.metaData->thumbnailImage = (BYTE*)ba1.data();
        newProfile.metaData->thumbnailImageSize = ba1.length();

        // set title thumbnail image
        QByteArray ba2;
        QBuffer buffer2(&ba2);
        buffer2.open(QIODevice::WriteOnly);
        QPixmap(":/Images/defaultTitleImage.png").save(&buffer2, "PNG");
        newProfile.metaData->titleThumbnailImage = (BYTE*)ba2.data();
        newProfile.metaData->titleThumbnailImageSize = ba2.length();

        newProfile.metaData->WriteMetaData();

        // create the account file
        QString accountTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

        FileIO accountIo(accountTempPath.toStdString(), true);

        BYTE tempBuffer[380] = {0};
        accountIo.Write(tempBuffer, 380);
        accountIo.Close();

        // Write the gamertag, and encrypt the file
        Account account(accountTempPath.toStdString(), false, newProfile.metaData->certificate.ownerConsoleType);
        account.SetGamertag(ui->txtGamertag->text().toStdWString());
        account.Save(newProfile.metaData->certificate.ownerConsoleType);

        // add the account file to the package
        newProfile.InjectFile(accountTempPath.toStdString(), "Account");

        // make a temporary copy of the dashboard gpd
        QString dashGpdTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        
        if (!QFile::exists(QtHelpers::ExecutingDirectory() + "/FFFE07D1.gpd"))
        {
            QString dashGpdPath = QFileDialog::getOpenFileName(this, tr("Open Dashboard GPD"),
                QtHelpers::DesktopLocation() + "/FFFE07D1.gpd", "Gpd File (*.gpd);;All Files (*.*)");

            if(dashGpdPath != "")
                QFile::copy(dashGpdPath, dashGpdTempPath);
            else
                throw string("FFFE07D1.gpd was not found.");
        }
        else
        {
            QFile::copy(QtHelpers::ExecutingDirectory() + "/FFFE07D1.gpd", dashGpdTempPath);
        }

        // parse the Gpd
        DashboardGpd dashGpd(dashGpdTempPath.toStdString());

        // change the gamerpicture key
        wstring picKey = L"fffe07d10002000" + QString::number(ui->listWidget->currentIndex().row()).toStdWString() + L"0001000" + QString::number(ui->listWidget->currentIndex().row()).toStdWString();
        dashGpd.gamerPictureKey.str = &picKey;
        dashGpd.WriteSettingEntry(dashGpd.gamerPictureKey);

        // if the avatar type is female, then we must change the avatar info setting
        if (ui->rdiFemale->isChecked())
        {
            // read in the setting
            memcpy(dashGpd.avatarInformation.binaryData.data, ___femaleAvatar_bin, ___femaleAvatar_bin_size);
            dashGpd.avatarInformation.binaryData.length = ___femaleAvatar_bin_size;
            dashGpd.WriteSettingEntry(dashGpd.avatarInformation);
        }

        // inject the image of the avatar
        QByteArray ba3;
        QBuffer buffer3(&ba3);
        buffer3.open(QIODevice::WriteOnly);
        if (ui->rdiMale->isChecked())
            QPixmap(":/Images/male avatar.png").save(&buffer3, "PNG");
        else
            QPixmap(":/Images/avatar female.png").save(&buffer3, "PNG");

        // inject the image
        ImageEntry image;
        image.image = new BYTE[ba3.length()];
        memcpy(image.image, (BYTE*)ba3.data(), ba3.length());
        image.length = ba3.length();

        dashGpd.CreateImageEntry(&image, AvatarImage);

        dashGpd.Close();

        // inject the dash gpd into the profile
        newProfile.InjectFile(dashGpdTempPath.toStdString(), "FFFE07D1.gpd");

        // create/inject the 64x64 image
        QString img64Path = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        ui->listWidget->currentItem()->icon().pixmap(64, 64).save(img64Path, "PNG");
        newProfile.InjectFile(img64Path.toStdString(), "tile_64.png");

        // create/inject the 32x32 image
        QString img32Path = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        ui->listWidget->currentItem()->icon().pixmap(32, 32).save(img32Path, "PNG");
        newProfile.InjectFile(img32Path.toStdString(), "tile_32.png");

        // create the avatar assets folder
        newProfile.CreateFolder("AvatarAssets");

        newProfile.Rehash();
        string path = QtHelpers::GetKVPath(newProfile.metaData->certificate.ownerConsoleType, this);

        if (path != "")
            newProfile.Resign(path);

        // delete the temp files
        QFile::remove(accountTempPath);
        QFile::remove(dashGpdTempPath);
        QFile::remove(img64Path);
        QFile::remove(img32Path);

        statusBar->showMessage("Created profile successfully", 3000);
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

    QChar prevChar = 0;
    for (int i = 1; i < gamertag.length(); i++)
    {
        if (gamertag.at(i) == ' ' && prevChar == ' ')
            return false;
        if (!gamertag.at(i).isLetterOrNumber() && gamertag.at(i) != ' ')
            return false;
        prevChar = gamertag.at(i);
    }
    return true;
}

void ProfileCreatorWizard::on_txtGamertag_textChanged(const QString & /* arg1 */)
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

void ProfileCreatorWizard::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Choose a place to create the profile", QtHelpers::DesktopLocation().replace("\\", "/") + "/" + QString::number(profileID, 16).toUpper());

    if (fileName != "")
        ui->lblSavePath->setText(fileName);
}

void ProfileCreatorWizard::on_radioButton_clicked(bool checked)
{
    if (checked)
        consoleType = Retail;
}

void ProfileCreatorWizard::on_radioButton_2_clicked(bool checked)
{
    if (checked)
        consoleType = DevKit;
}
