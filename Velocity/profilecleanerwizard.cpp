#include "profilecleanerwizard.h"
#include "ui_profilecleanerwizard.h"

ProfileCleanerWizard::ProfileCleanerWizard(QWidget *parent) :
    QWizard(parent), ui(new Ui::ProfileCleanerWizard), profileOpened(false), op(Dust)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

#ifdef __APPLE__
    ui->label->setText("<font size=3>" + ui->label->text() + "</font>");
#elif _WIN32
    ui->label->setFont(QFont("MS Shell Dlg 2", 10));
    setFixedSize(588, 480);
#endif

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(onCurrentIdChanged(int)));
}

ProfileCleanerWizard::~ProfileCleanerWizard()
{
    delete ui;
}

void ProfileCleanerWizard::onCurrentIdChanged(int id)
{
    switch (id)
    {
        case 1:
            button(QWizard::NextButton)->setEnabled(profileOpened);
            break;
        case 3:
            button(QWizard::BackButton)->setEnabled(false);
            clean();
            break;
    }
}

void ProfileCleanerWizard::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Choose a profile to clean",
            QtHelpers::DefaultLocation());
    if (filePath == "")
        return;

    QFileInfo fileInfo(filePath);
    ui->lblStartSize->setText(QString::fromStdString(ByteSizeToString(fileInfo.size())));
    initialSize = fileInfo.size();

    try
    {
        profile = new StfsPackage(filePath.toStdString());
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error",
                "An error occurred while opening your profile.\n\n" + QString::fromStdString(error));
        return;
    }

    if (profile->metaData->contentType != Profile)
    {
        QMessageBox::warning(this, "Invalid Profile", "The package you opened is not a profile.");
        delete profile;
        return;
    }
    else if (!profile->FileExists("FFFE07D1.gpd") || !profile->FileExists("Account"))
    {
        QMessageBox::warning(this, "Invalid Profile", "The profile you opened is missing essential files.");
        delete profile;
        return;
    }

    profileOpened = true;
    profilePath = filePath;
    ui->label_9->setText(profilePath);
    button(QWizard::NextButton)->setEnabled(true);
}

void ProfileCleanerWizard::clean()
{
    button(QWizard::FinishButton)->setEnabled(false);

    // extract all of the files
    directory = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}",
                "").replace("-", "") + "/";
    QDir d(directory);
    d.mkdir(directory);
    StfsFileListing f = profile->GetFileListing();
    extractAll(&f, directory);
    qDebug() << "Extraction complete";

    QStringList exceptions;
    exceptions.push_back("FFFE07D1.gpd");
    exceptions.push_back("FFFE07DE.gpd");
    exceptions.push_back("584D07D1.gpd");

    // iterate through all of the Gpds in the profile
    QFileInfoList files = d.entryInfoList();
    for (int i = 0; i < files.size(); i++)
    {
        if (files.at(i).fileName().mid(files.at(i).fileName().lastIndexOf(".")) == ".gpd")
        {
            try
            {
                GpdBase gpd(files.at(i).absoluteFilePath().toStdString());

                if (op == Purify && !exceptions.contains(files.at(i).fileName()))
                {
                    DWORD offset = 0, settingCount = gpd.settings.size();
                    for (DWORD i = 0; i < settingCount; i++)
                    {
                        if (gpd.settings.at(offset).entry.id == GamercardTitleAchievementsEarned ||
                                gpd.settings.at(offset).entry.id == GamercardTitleCredEarned)
                            offset++;
                        else
                            gpd.DeleteSettingEntry(gpd.settings.at(offset));
                    }
                }
                if ((op == Purify || op == Sweep) && !exceptions.contains(files.at(i).fileName()))
                {
                    DWORD offset = 0, imageCount = gpd.images.size();
                    for (DWORD i = 0; i < imageCount; i++)
                    {
                        if (gpd.images.at(offset).entry.id == TitleInformation)
                            offset++;
                        else
                            gpd.DeleteImageEntry(gpd.images.at(offset));
                    }
                }

                gpd.Clean();
                QApplication::processEvents();
            }
            catch (string error)
            {
                QMessageBox::critical(this, "Clean Error",
                        "An error has occurred while cleaning your profile.\n\n" + QString::fromStdString(error));

                deleteAllRecursive(QDir(directory));
                d.rmdir(directory);
                delete profile;

                return;
            }
        }
    }

    // Rebuild the profile
    QString newProfilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
    StfsPackage newProfile(newProfilePath.toStdString(), StfsPackageCreate);

    // Copy all the metadata over to the new profile
    newProfile.metaData->certificate.publicKeyCertificateSize = profile->metaData->certificate.publicKeyCertificateSize;

    // Use assignment for std::string instead of memcpy
    newProfile.metaData->certificate.ownerConsolePartNumber = profile->metaData->certificate.ownerConsolePartNumber;
    newProfile.metaData->certificate.ownerConsoleType = profile->metaData->certificate.ownerConsoleType;
    newProfile.metaData->certificate.consoleTypeFlags = profile->metaData->certificate.consoleTypeFlags;

    // Use assignment for std::string
    newProfile.metaData->certificate.dateGeneration = profile->metaData->certificate.dateGeneration;

    // Use std::copy for arrays
    std::copy(profile->metaData->certificate.ownerConsoleID, profile->metaData->certificate.ownerConsoleID + 5, newProfile.metaData->certificate.ownerConsoleID);
    std::copy(profile->metaData->consoleID, profile->metaData->consoleID + 5, newProfile.metaData->consoleID);

    // Use assignment for std::wstring
    newProfile.metaData->displayDescription = profile->metaData->displayDescription;
    newProfile.metaData->displayName = profile->metaData->displayName;

    // Use std::copy for licenseData array
    std::copy(profile->metaData->licenseData, profile->metaData->licenseData + 0x10, newProfile.metaData->licenseData);

    // Copy other metadata fields
    newProfile.metaData->contentSize = profile->metaData->contentSize;
    newProfile.metaData->contentType = Profile;
    newProfile.metaData->titleID = profile->metaData->titleID;

    // Use std::copy for deviceID array
    std::copy(profile->metaData->deviceID, profile->metaData->deviceID + 20, newProfile.metaData->deviceID);

    // Use assignment for std::wstring
    newProfile.metaData->publisherName = profile->metaData->publisherName;

    // Handle thumbnail images
    newProfile.metaData->thumbnailImageSize = profile->metaData->thumbnailImageSize;
    newProfile.metaData->titleThumbnailImageSize = profile->metaData->titleThumbnailImageSize;

    // Allocate and copy thumbnail images
    newProfile.metaData->thumbnailImage = new BYTE[profile->metaData->thumbnailImageSize];
    newProfile.metaData->titleThumbnailImage = new BYTE[profile->metaData->titleThumbnailImageSize];

    // Use std::copy for thumbnail images
    std::copy(profile->metaData->thumbnailImage, profile->metaData->thumbnailImage + profile->metaData->thumbnailImageSize, newProfile.metaData->thumbnailImage);
    std::copy(profile->metaData->titleThumbnailImage, profile->metaData->titleThumbnailImage + profile->metaData->titleThumbnailImageSize, newProfile.metaData->titleThumbnailImage);

    // Use assignment for std::wstring
    newProfile.metaData->titleName = profile->metaData->titleName;

    newProfile.metaData->transferFlags = profile->metaData->transferFlags;
    newProfile.metaData->version = profile->metaData->version;

    newProfile.metaData->WriteMetaData();


    // inject all of the old files into the profile
    injectAll(&newProfile, directory, "");

    // delete the old profile
    delete profile;

    QApplication::processEvents();

    deleteAllRecursive(QDir(directory));
    d.rmdir(directory);
    qDebug() << "Deletion complete";

    // fix the profile
    newProfile.Rehash();
    newProfile.Resign(QtHelpers::GetKVPath(newProfile.metaData->certificate.ownerConsoleType, this));

    newProfile.Close();

    QFileInfo info(newProfilePath);
    ui->lblEndSize->setText(QString::fromStdString(ByteSizeToString(info.size())));
    ui->lblDataRemoved->setText(QString::fromStdString(ByteSizeToString(initialSize - info.size())));

    // move the cleaned profile to the old location
    QFile::remove(profilePath);
    QFile::rename(newProfilePath, profilePath);

    // update the ui
    ui->progressBar->setVisible(false);
    ui->wizardPage_2->setTitle("Finished");
    ui->wizardPage_2->setSubTitle("Your profile has been cleaned successfully");
    button(QWizard::FinishButton)->setEnabled(true);
}

void ProfileCleanerWizard::extractAll(StfsFileListing *f, QString parentDirectory)
{
    // extract all files
    for (DWORD i = 0; i < f->fileEntries.size(); i++)
    {
        profile->ExtractFile(&f->fileEntries.at(i),
                parentDirectory.toStdString() + f->fileEntries.at(i).name);
        QApplication::processEvents();
    }
    // create all folders
    for (DWORD i = 0; i < f->folderEntries.size(); i++)
    {
        QDir d;
        d.mkpath(parentDirectory + QString::fromStdString(f->folderEntries.at(i).folder.name) + "/");
        if (f->folderEntries.at(i).fileEntries.size() != 0)
            extractAll(&f->folderEntries.at(i),
                       parentDirectory + QString::fromStdString(f->folderEntries.at(i).folder.name) + "/");
    }
}

void ProfileCleanerWizard::deleteAllRecursive(QDir directory)
{
    QFileInfoList files = directory.entryInfoList(QDir::Files);
    for (int i = 0; i < files.size(); i++)
        QFile::remove(files.at(i).absoluteFilePath());
    QApplication::processEvents();
    QFileInfoList dirs = directory.entryInfoList(QDir::Dirs);
    for (int i = 2; i < dirs.size(); i++)
    {
        QDir d;
        deleteAllRecursive(QDir(dirs.at(i).absoluteFilePath() + "/"));
        d.rmdir(dirs.at(i).absoluteFilePath());
    }
}

void ProfileCleanerWizard::injectAll(StfsPackage *profile, QDir currentDirectory,
        QString currentStfsDir)
{
    try
    {
        QFileInfoList files = currentDirectory.entryInfoList(QDir::Files);
        for (int i = 0; i < files.size(); i++)
        {
            profile->InjectFile(files.at(i).absoluteFilePath().toStdString(),
                    currentStfsDir.toStdString() + files.at(i).fileName().toStdString());
            QApplication::processEvents();
        }
        QFileInfoList dirs = currentDirectory.entryInfoList(QDir::Dirs);
        for (int i = 2; i < dirs.size(); i++)
        {
            qDebug() << "dirname: " << dirs.at(i).baseName();
            profile->CreateFolder(currentStfsDir.toStdString() + dirs.at(i).baseName().toStdString());
            injectAll(profile, QDir(currentDirectory.absolutePath() + "/" + dirs.at(i).fileName() + "/"),
                      currentStfsDir + dirs.at(i).fileName() + "\\");
        }
    }
    catch (string error)
    {
        qDebug() << "Clean error: " << QString::fromStdString(error);
        return;
    }
}

void ProfileCleanerWizard::on_radioButton_toggled(bool checked)
{
    if (checked)
        op = Dust;
}

void ProfileCleanerWizard::on_radioButton_2_toggled(bool checked)
{
    if (checked)
        op = Sweep;
}

void ProfileCleanerWizard::on_radioButton_3_toggled(bool checked)
{
    if (checked)
        op = Purify;
}
