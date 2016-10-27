#include "profileeditor.h"
#include "ui_profileeditor.h"

ProfileEditor::ProfileEditor(QStatusBar *statusBar, StfsPackage *profile, bool dispose, QWidget *parent) :
    QDialog(parent), ui(new Ui::ProfileEditor), profile(profile), PEC(NULL), dashGpd(NULL), account(NULL), downloader(NULL), dispose(dispose), statusBar(statusBar)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);
    unlockEverything = new QPushButton("Unlock Everything", this);
    unlockEverything->setIcon(QIcon(":/Images/star.png"));
    unlockEverything->setHidden(true);
    ui->tabWidget->setCornerWidget(unlockEverything);
    connect(unlockEverything, SIGNAL(clicked()), this, SLOT(onUnlockEverything()));

#ifdef __linux__
    QFont f = ui->lblAchName->font();
    f.setPointSize(13);
    ui->lblAchName->setFont(f);
    ui->lblAwName->setFont(f);

    setMinimumHeight(485);

    int btnSize = 37;

    unlockEverything->setFont(ui->btnUnlockAllAchvs->font());
#elif __APPLE__
    int btnSize = 37;
    setMinimumSize(1000, 520);
    ui->groupBox_3->setMaximumWidth(395);
#elif __WIN32__
    int btnSize = 32;
    ui->lblAchName->setFont(QFont("Arial", 10));
    ui->lblAwName->setFont(QFont("Arial", 10));
#endif

    ui->btnExtractGPD->setMinimumHeight(btnSize);
    ui->btnExtractGPD_2->setMinimumHeight(btnSize);
    ui->btnCreateAch->setMinimumHeight(btnSize);
    ui->btnUnlockAllAchvs->setMinimumHeight(btnSize);
    ui->btnAwardShowAll->setMaximumHeight(btnSize);
    ui->btnUnlockAllAwards->setMaximumHeight(btnSize);

    ui->achievementsList->header()->resizeSection(0, 350);
    ui->achievementsList->header()->resizeSection(1, 125);
    ui->achievementsList->header()->resizeSection(2, 125);

    ui->avatarAwardsList->header()->resizeSection(0, 350);
    ui->avatarAwardsList->header()->resizeSection(1, 125);
    ui->avatarAwardsList->header()->resizeSection(2, 125);

    gameBoxArtManager = new QNetworkAccessManager(this);
    connect(gameBoxArtManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedBoxArt(QNetworkReply*)));

    awardBoxArtManager = new QNetworkAccessManager(this);
    connect(awardBoxArtManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedAwBoxArt(QNetworkReply*)));

    awardThumbnailManager = new QNetworkAccessManager(this);
    connect(awardThumbnailManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedAwImg(QNetworkReply*)));

    // verify that the package is a profile
    if (profile->metaData->contentType != Profile)
    {
        ok = false;
        QMessageBox::critical(this, "Invalid Package", "Package opened isn't a profile.\n");
        return;
    }

    // make sure the dashboard gpd exists
    if (!profile->FileExists("FFFE07D1.gpd"))
    {
        ok = false;
        QMessageBox::critical(this, "File Not Found", "Dashboard Gpd, FFFE07D1.gpd, wasn't found.\n");
        return;
    }

    // make sure the account file exists
    if (!profile->FileExists("Account"))
    {
        ok = false;
        QMessageBox::critical(this, "File Not Found", "The Account file wasn't found.\n");
        return;
    }

    // setup the ui context menu stuff
    connect(ui->imgAch, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onSaveAchievementThumbnail(QPoint)));
    connect(ui->imgAw, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onSaveAvatarAwardThumbnail(QPoint)));

    ///////////////////////////////////
    // LOAD FRONT PAGE
    ///////////////////////////////////

    try
    {
        statusBar->showMessage("Loading profile...");

        // extract the account file
        accountTempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        profile->ExtractFile("Account", accountTempPath);
        tempFiles.push_back(accountTempPath);

        // parse the account file
        account = new Account(accountTempPath, true, profile->metaData->certificate.ownerConsoleType);

        // load all of the account information
        ui->txtGamertag->setText(QString::fromStdWString(account->GetGamertag()));
        ui->lblLanguage->setText(QString::fromStdString(AccountHelpers::ConsoleLanguageToString(account->GetLanguage())));
        ui->lblSubscriptionTeir->setText(QString::fromStdString(AccountHelpers::SubscriptionTeirToString(account->GetSubscriptionTeir())));
        ui->lblParentalControlled->setText(((account->IsParentalControlled()) ? "Yes" : "No"));
        ui->lblCreditCard->setText(((account->IsPaymentInstrumentCreditCard()) ? "Yes" : "No"));
        ui->lblXUID->setText(QString::number(account->GetXUID(), 16).toUpper());
        ui->chxLIVE->setCheckState((Qt::CheckState)(account->IsLiveEnabled() << 1));
        ui->chxRecovering->setCheckState((Qt::CheckState)(account->IsRecovering() << 1));
        ui->chxPasscode->setCheckState((Qt::CheckState)(account->IsPasscodeEnabled() << 1));
        ui->cmbxConsoleType->setCurrentIndex((profile->metaData->certificate.ownerConsoleType == DevKit) ? 0 : 1);

        // load passcode
        BYTE passCode[4];
        account->GetPasscode(passCode);
        ui->cmbxPass1->setCurrentIndex(passCode[0]);
        ui->cmbxPass2->setCurrentIndex(passCode[1]);
        ui->cmbxPass3->setCurrentIndex(passCode[2]);
        ui->cmbxPass4->setCurrentIndex(passCode[3]);

        switch (account->GetServiceProvider())
        {
            case ProductionNet:
                ui->cmbxNetwork->setCurrentIndex(0);
                break;
            case PartnerNet:
                ui->cmbxNetwork->setCurrentIndex(1);
                break;
            case LiveDisabled:
                ui->cmbxNetwork->setEnabled(false);
                break;
            default:
                ui->cmbxNetwork->addItem(QString::fromStdString(AccountHelpers::XboxLiveServiceProviderToString(account->GetServiceProvider())));
                break;
        }

        // disable the controls that won't be used
        ui->cmbxPass1->setEnabled(account->IsPasscodeEnabled());
        ui->cmbxPass2->setEnabled(account->IsPasscodeEnabled());
        ui->cmbxPass3->setEnabled(account->IsPasscodeEnabled());
        ui->cmbxPass4->setEnabled(account->IsPasscodeEnabled());
        ui->cmbxNetwork->setEnabled(account->IsLiveEnabled());

        // populate the years on live combo box
        for (DWORD i = 0; i <= 20; i++)
            ui->cmbxTenure->addItem(QString::number(i));

        // populate the regions combo box
        for (DWORD i = 0; i < 109; i++)
            ui->cmbxRegion->addItem(regions[i].name);

        // load the gamerpicture
        QByteArray imageBuff((char*)profile->metaData->thumbnailImage, (size_t)profile->metaData->thumbnailImageSize);
        ui->imgGamerpicture->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));

        // extract the dashboard gpd
        //dashGpdTempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        //tempFiles.push_back(dashGpdTempPath);
        //profile->ExtractFile("FFFE07D1.gpd", dashGpdTempPath);

        // parse the dashboard gpd
        StfsIO *dashGpdIO = profile->GetStfsIO("FFFE07D1.gpd");
        dashGpd = new DashboardGpd((BaseIO*)dashGpdIO);

        // load all the goodies
        if (dashGpd->gamerName.entry.type == 0)
            addToDashGpd(&dashGpd->gamerName, UnicodeString, GamercardUserName);
        else
            ui->txtName->setText(QString::fromStdWString(*dashGpd->gamerName.str));

        if (dashGpd->gamerscoreUnlocked.entry.type == 0)
        {
            addToDashGpd(&dashGpd->gamerscoreUnlocked, Int32, GamercardCred);
            ui->lblGamerscore->setText("0G");
        }
        else
            ui->lblGamerscore->setText(QString::number(dashGpd->gamerscoreUnlocked.int32) + "G");

        if (dashGpd->achievementsUnlocked.entry.type == 0)
            addToDashGpd(&dashGpd->achievementsUnlocked, Int32, GamercardAchievementsEarned);

        if (dashGpd->motto.entry.type == 0)
            addToDashGpd(&dashGpd->motto, UnicodeString, GamercardMotto);
        else
            ui->txtMotto->setText(QString::fromStdWString(*dashGpd->motto.str));

        if (dashGpd->gamerLocation.entry.type == 0)
            addToDashGpd(&dashGpd->gamerLocation, UnicodeString, GamercardUserLocation);
        else
            ui->txtLocation->setText(QString::fromStdWString(*dashGpd->gamerLocation.str));

        if (dashGpd->gamerzone.entry.type == 0)
            addToDashGpd(&dashGpd->gamerzone, Int32, GamercardZone);
        else
            ui->cmbxGamerzone->setCurrentIndex(dashGpd->gamerzone.int32);

        if (dashGpd->gamercardRegion.entry.type == 0)
            addToDashGpd(&dashGpd->gamercardRegion, Int32, GamercardRegion);
        else
        {
            // find the index of the gamer's region
            for (int i = 0; i < 109; i++)
               if (regions[i].value == dashGpd->gamercardRegion.int32)
                   ui->cmbxRegion->setCurrentIndex(i);
        }

        if (dashGpd->yearsOnLive.entry.type == 0)
            addToDashGpd(&dashGpd->yearsOnLive, Int32, YearsOnLive);
        else
            ui->cmbxTenure->setCurrentIndex(dashGpd->yearsOnLive.int32);

        if (dashGpd->reputation.entry.type == 0)
            addToDashGpd(&dashGpd->reputation, Float, GamercardRep);
        else
            ui->spnRep->setValue(dashGpd->reputation.floatData);

        if (dashGpd->gamerBio.entry.type == 0)
            addToDashGpd(&dashGpd->gamerBio, UnicodeString, GamercardUserBio);
        else
            ui->txtBio->setPlainText(QString::fromStdWString(*dashGpd->gamerBio.str));

        // load the avatar image
        if (dashGpd->avatarImage.entry.type != 0)
        {
            QByteArray imageBuff((char*)dashGpd->avatarImage.image, (size_t)dashGpd->avatarImage.length);
            ui->imgAvatar->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));
        }
        else
            ui->imgAvatar->setPixmap(QPixmap::fromImage(QImage(":/Images/avatar-body.png")));
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error has occurred while loading the profile.\n\n" + QString::fromStdString(error));
        ok = false;
        return;
    }
    catch (...)
    {
        QMessageBox::critical(this, "Error", "An unknown error has occurred while loading the profile.");
        ok = false;
        return;
    }

    QApplication::processEvents();


    ////////////////////////////
    // LOAD ACHIEVEMENTS
    ////////////////////////////

    QMap<QString, GpdPaths> paths;

    // load all the games played
    for (DWORD i = 0; i < dashGpd->gamesPlayed.size(); i++)
    {
        // if the game doesn't have any achievements, no need to load it
        if (dashGpd->gamesPlayed.at(i).achievementCount == 0 && dashGpd->gamesPlayed.at(i).avatarAwardCount == 0)
            continue;

        QString titleIDStr = QString::number(dashGpd->gamesPlayed.at(i).titleID, 16).toUpper();
        // make sure the corresponding gpd exists
        QString gpdName = titleIDStr + ".gpd";
        if (!profile->FileExists(gpdName.toStdString()))
        {
            ok = false;
            QMessageBox::critical(this, "File Not Found", "Couldn't find file \"" + gpdName + "\".");
            return;
        }

        // extract the gpd
        string tempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        profile->ExtractFile(gpdName.toStdString(), tempPath);
        tempFiles.push_back(tempPath);

        QString newPath = QString::fromStdString(tempPath + "_C");
        QFile::copy(QString::fromStdString(tempPath), newPath);

        paths[titleIDStr].gameGpd = newPath;
        paths[titleIDStr].awardGpd = "";

        // parse the gpd
        GameGpd *gpd;
        try
        {
            gpd = new GameGpd(tempPath);
        }
        catch (string error)
        {
            ok = false;
            QMessageBox::critical(this, "File Load Error", "Error loading game Gpd, '" + gpdName + "'.\n\n" + QString::fromStdString(error));
            return;
        }

        // if there aren't any achievements for it, then don't add it to the game list
        if (dashGpd->gamesPlayed.at(i).achievementCount != 0)
        {
            GameEntry g = { gpd, &dashGpd->gamesPlayed.at(i), false, tempPath, gpdName.toStdString() };
            games.push_back(g);
        }

        // if there are avatar awards then add it to the vector
        if (dashGpd->gamesPlayed.at(i).avatarAwardCount != 0)
        {
            AvatarAwardGameEntry a = { gpd, &dashGpd->gamesPlayed.at(i), NULL, false, string(""), string("") };
            aaGames.push_back(a);
        }

        // create the tree widget item
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdWString(gpd->gameName.ws));

        // set the thumbnail
        if (gpd->thumbnail.image != NULL)
        {
            QByteArray imageBuff((char*)gpd->thumbnail.image, (size_t)gpd->thumbnail.length);
            item->setIcon(0, QIcon(QPixmap::fromImage(QImage::fromData(imageBuff))));
        }
        else
            item->setIcon(0, QIcon(QPixmap(":/Images/HiddenAchievement.png")));

        // add the item to the list
        if (dashGpd->gamesPlayed.at(i).achievementCount != 0)
            ui->gamesList->insertTopLevelItem(ui->gamesList->topLevelItemCount(), item);

        // add it to the avatar award game list if needed
        if (dashGpd->gamesPlayed.at(i).avatarAwardCount != 0)
            ui->aaGamelist->insertTopLevelItem(ui->aaGamelist->topLevelItemCount(), new QTreeWidgetItem(*item));

        QApplication::processEvents();
    }

    if (ui->gamesList->topLevelItemCount() >= 1)
    {
        ui->gamesList->setCurrentItem(ui->gamesList->topLevelItem(0));
        ui->gamesList->topLevelItem(0)->setSelected(true);

        ui->achievementsList->setCurrentItem(ui->achievementsList->topLevelItem(0));
        ui->achievementsList->topLevelItem(0)->setSelected(true);
    }
    else
        ui->tabAchievements->setEnabled(false);

    /////////////////////////
    // LOAD AVATAR AWARDS
    /////////////////////////

    // make sure the PEC file exists
    bool pecExists = profile->FileExists("PEC");
    if (!pecExists && aaGames.size() != 0)
    {
        ok = false;
        QMessageBox::critical(this, "File Not Found", "Games have been found with avatar awards, but no PEC file exists.\n");
        ui->tabAvatarAwards->setEnabled(false);
        return;
    }
    else if (!pecExists)
    {
        ui->tabAvatarAwards->setEnabled(false);
        statusBar->showMessage("Profile loaded successfully", 3000);
        return;
    }

    try
    {
        // extract the PEC file
        //pecTempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        //profile->ExtractFile("PEC", pecTempPath);
        //tempFiles.push_back(pecTempPath);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error has occurred while extracting the PEC.\n\n" + QString::fromStdString(error));
        ok = false;
        return;
    }

    // parse the PEC STFS package
    try
    {
        StfsIO *pecIO = this->profile->GetStfsIO("PEC");
        PEC = new StfsPackage((BaseIO*)pecIO, StfsPackagePEC);
    }
    catch (string error)
    {
        ok = false;
        QMessageBox::critical(this, "File Load Error", "Error loading PEC file.\n\n" + QString::fromStdString(error));
        return;
    }

    // extract and parse all of the Gpds in the PEC
    for (DWORD i = 0; i < aaGames.size(); i++)
    {
        QString titleIDStr = QString::number(aaGames.at(i).titleEntry->titleID, 16).toUpper();

        // get the name of the Gpd in the PEC
        QString gpdName = titleIDStr + ".gpd";

        // make sure the Gpd exists
        if (!PEC->FileExists(gpdName.toStdString()))
        {
            ok = false;
            QMessageBox::critical(this, "File Not Error", "Avatar Award Gpd '" + gpdName + "' was not found in the PEC.\n");
            return;
        }

        // extract the avatar award Gpd
        string tempGpdName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        tempFiles.push_back(tempGpdName);
        PEC->ExtractFile(gpdName.toStdString(), tempGpdName);

        QString newPath = QString::fromStdString(tempGpdName + "_C");
        QFile::copy(QString::fromStdString(tempGpdName), newPath);
        paths[titleIDStr].awardGpd = newPath;

        // parse the avatar award gpd
        AvatarAwardGpd *gpd;
        try
        {
            gpd = new AvatarAwardGpd(tempGpdName);
        }
        catch (string error)
        {
            ok = false;
            QMessageBox::critical(this, "File Load Error", "Error loading the Avatar Award Gpd '" + gpdName + "'.\n\n" + QString::fromStdString(error));
            return;
        }
        aaGames.at(i).gpd = gpd;
        aaGames.at(i).tempFileName = tempGpdName;
        aaGames.at(i).gpdName = gpdName.toStdString();

        QApplication::processEvents();
    }

    if (aaGames.size() >= 1)
    {
        ui->aaGamelist->setCurrentItem(ui->aaGamelist->topLevelItem(0));
        ui->aaGamelist->topLevelItem(0)->setSelected(true);

        ui->avatarAwardsList->setCurrentItem(ui->avatarAwardsList->topLevelItem(0));
        ui->avatarAwardsList->topLevelItem(0)->setSelected(true);
    }
    else
        ui->tabAvatarAwards->setEnabled(false);

    // setup the context menu
    ui->avatarAwardsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->avatarAwardsList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showAvatarContextMenu(QPoint)));

    statusBar->showMessage("Profile loaded successfully", 3000);

    QStringList gameGpds, awardGpds, titleIDs;
    foreach (QString key, paths.keys())
    {
        titleIDs.push_back(key);
        gameGpds.push_back(paths[key].gameGpd);
        awardGpds.push_back(paths[key].awardGpd);
    }

    uploader = new GpdUploader(gameGpds, awardGpds, titleIDs, true, this);
    
    ok = true;
}

void ProfileEditor::showAvatarContextMenu(QPoint point)
{
    QPoint globalPos = ui->avatarAwardsList->mapToGlobal(point);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/download.png"), "Download Award");
    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == NULL)
        return;
    else if (selectedItem->text() == "Download Award")
    {
        int button = QMessageBox::question(this, "Warning", "You are about to download an avatar award file. Unfortunatly the awards downloaded like this will only work on a JTAG/RGH/Dev.\n\nDo you want to continue?",
                                                                   QMessageBox::Yes, QMessageBox::No);

        if (button != QMessageBox::Yes)
            return;

        QString titleID = QString::number(aaGames.at(ui->aaGamelist->currentIndex().row()).titleEntry->titleID, 16);
        QString guid = QString::fromStdString(AvatarAwardGpd::GetGUID(&aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row())));

        // get a path for the new asset
        QString assetFileName = guid;
        assetFileName = assetFileName.replace("-", "").toUpper();
        assetSavePath = QFileDialog::getSaveFileName(this, "Choose a place to save the asset", QtHelpers::DesktopLocation() + "/" + assetFileName, "*");

        if (assetSavePath == "")
            return;

        downloader = new AvatarAssetDownloader(titleID, guid);
        connect(downloader, SIGNAL(FinishedDownloading()), this, SLOT(onAssetsDoneDownloading()));
        downloader->BeginDownload();
        statusBar->showMessage("Requesting asset...", 3000);
    }
}

void ProfileEditor::onAssetsDoneDownloading()
{
    try
    {
        struct AvatarAward *award = &aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row());

        StfsPackage newAsset(assetSavePath.toStdString(), StfsPackageCreate | StfsPackageFemale);
        newAsset.metaData->magic = PIRS;
        newAsset.metaData->certificate.ownerConsoleType = profile->metaData->certificate.ownerConsoleType;

        // unlock the asset
        newAsset.metaData->licenseData[0].bits = 1;

        // set all the needed metadata
        newAsset.metaData->contentType = AvatarItem;
        newAsset.metaData->titleID = award->titleID;

        // not sure why i have to do this, but it triggors an 'Unknown Signal' from the OS if I don't
        std::wstring *w = new std::wstring(award->name);
        memcpy(&newAsset.metaData->displayName, w, sizeof(std::wstring));

        newAsset.metaData->subCategory = award->subcategory;
        newAsset.metaData->colorizable = award->colorizable;
        QtHelpers::ParseHexStringBuffer(downloader->GetGUID().replace("-", "").toUpper(), newAsset.metaData->guid, 0x10);
        newAsset.metaData->skeletonVersion = (downloader->GetV2TempPath() != "") ? Natal : Nxe;
        newAsset.metaData->WriteMetaData();

        try { newAsset.InjectFile(downloader->GetV1TempPath().toStdString(), "asset.bin"); }
        catch (...) { }

        try
        { newAsset.InjectFile(downloader->GetV2TempPath().toStdString(), "asset_v2.bin"); }
        catch (...) { }

        // get the thumbnail
        QByteArray baThumb;
        QBuffer buffThumb(&baThumb);
        buffThumb.open(QIODevice::WriteOnly);
        ui->imgAw->pixmap()->save(&buffThumb, "PNG");
        newAsset.InjectData((BYTE*)baThumb.data(), baThumb.length(), "icon.png");

        QByteArray baScaled;
        QBuffer buffScaled(&baScaled);
        buffScaled.open(QIODevice::WriteOnly);
        QPixmap scaled = ui->imgAw->pixmap()->scaled(QSize(64, 64));
        scaled.save(&buffScaled, "PNG");
        newAsset.metaData->thumbnailImage = (BYTE*)baScaled.data();
        newAsset.metaData->thumbnailImageSize = baScaled.length();

        GameGpd *gpd = aaGames.at(ui->aaGamelist->currentIndex().row()).gameGpd;
        newAsset.metaData->titleThumbnailImage = gpd->thumbnail.image;
        newAsset.metaData->titleThumbnailImageSize = gpd->thumbnail.length;

        newAsset.Rehash();

        delete downloader;

        statusBar->showMessage("Asset downloaded successfully", 3000);
        QMessageBox::information(this, "Asset Downloaded", "Successfully downloaded the avatar asset.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while creating the asset package.\n\n" + QString::fromStdString(error));
    }
    catch (...)
    {
        QMessageBox::critical(this, "Error", "An unknown error occurred while creating the asset package.");
    }
}

void ProfileEditor::addToDashGpd(SettingEntry *entry, SettingEntryType type, UINT64 id)
{
    entry->type = type;

    switch (type)
    {
        case Context:
            break;
        case Int32:
        case Int64:
        case Float:
        case Double:
        case TimeStamp:
            entry->int32 = 0;
            break;
        case Binary:
            entry->binaryData.length = 0;
            entry->binaryData.data = NULL;
            break;
        case UnicodeString:
            entry->str = new wstring(L"");
            break;
    }

    dashGpd->CreateSettingEntry(entry, id);
}

ProfileEditor::~ProfileEditor()
{
    if (ok)
    {
        try
        {
            saveAll();
            statusBar->showMessage("Saved all changes", 3000);
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while saving the profile.\n\n" + QString::fromStdString(error));
        }
    }

    // free all the game gpd memory
    for (DWORD i = 0; i < games.size(); i++)
        delete games.at(i).gpd;

    // free all of the avatar award gpds
    for (DWORD i = 0; i < aaGames.size(); i++)
    {
        if (aaGames.at(i).titleEntry->achievementCount == 0)
           delete aaGames.at(i).gameGpd;
        delete aaGames.at(i).gpd;
    }

    if (dashGpd != NULL)
    {
        dashGpd->Close();
        delete dashGpd;
    }

    if (account != NULL)
        delete account;

    if (dispose)
        delete profile;

    // delete all of the temp files
    for (DWORD i = 0; i < tempFiles.size(); i++)
    {
        QFile::remove(QString::fromStdString(tempFiles.at(i)));
        QFile::remove(QString::fromStdString(tempFiles.at(i)) + "_C");
    }

    delete ui;
}

void ProfileEditor::onSaveAchievementThumbnail(QPoint p)
{
    saveImage(p, ui->imgAch);
}

void ProfileEditor::onSaveAvatarAwardThumbnail(QPoint p)
{
    saveImage(p, ui->imgAw);
}

void ProfileEditor::on_gamesList_itemSelectionChanged()
{
    try
    {
        loadGameInfo(ui->gamesList->currentIndex().row());
        loadAchievementInfo(ui->gamesList->currentIndex().row(), 0);
    }
    catch (std::string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while loading a game.\n\n" + QString::fromStdString(error));
    }
}

void ProfileEditor::loadGameInfo(int index)
{
    if (index < 0)
        return;

    GameGpd *curGpd = games.at(index).gpd;
    ui->achievementsList->clear();

    // load all the achievements
    for (DWORD i = 0; i < curGpd->achievements.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->achievementsList);
        item->setText(0, QString::fromStdWString(curGpd->achievements.at(i).name).replace("\n", ""));
        item->setText(1, QString::fromStdString(XdbfHelpers::GetAchievementState(&curGpd->achievements.at(i))));
        item->setText(2, QString::number(curGpd->achievements.at(i).gamerscore));

        ui->achievementsList->insertTopLevelItem(ui->achievementsList->topLevelItemCount(), item);
    }

    ui->achievementsList->setCurrentItem(ui->achievementsList->topLevelItem(0));
    ui->achievementsList->topLevelItem(0)->setSelected(true);

    TitleEntry *title = games.at(index).titleEntry;

    ui->lblGameName->setText("<span style=\"color:#4f4f4f;\">" + QString::fromStdWString(title->gameName) + "</span>");
    ui->lblGameTitleID->setText("<span style=\"color:#4f4f4f;\">" + QString::number(title->titleID, 16).toUpper() + "</span>");
    if (title->lastPlayed == 0)
        ui->lblGameLastPlayed->setText("<span style=\"color:#4f4f4f;\">N/A</span>");
    else
        ui->lblGameLastPlayed->setText("<span style=\"color:#4f4f4f;\">" + QDateTime::fromTime_t(title->lastPlayed).toString(Qt::SystemLocaleShortDate) + "</span>");
    ui->lblGameAchvs->setText("<span style=\"color:#4f4f4f;\">" + QString::number(title->achievementsUnlocked) + " out of " + QString::number(title->achievementCount) + " unlocked" + "</span>");
    ui->lblGameGamerscore->setText("<span style=\"color:#4f4f4f;\">" + QString::number(title->gamerscoreUnlocked) + " out of " + QString::number(title->totalGamerscore) + " unlocked" + "</span>");

    string tmp = DashboardGpd::GetSmallBoxArtURL(title);
    gameBoxArtManager->get(QNetworkRequest(QUrl(QString::fromStdString(tmp))));
}

void ProfileEditor::saveImage(QPoint p, QLabel *imgLabel)
{
    if (imgLabel->pixmap()->isNull())
        return;

    QPoint globalPos = imgLabel->mapToGlobal(p);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/save.png"), "Save Image");
    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == NULL)
        return;
    else if (selectedItem->text() == "Save Image")
    {
        QString saveFileName = QFileDialog::getSaveFileName(this, "Choose a place to save the thumbnail", QtHelpers::DesktopLocation() + "\\thumbnail.png", "*.png");

        if (saveFileName == "")
            return;

        imgLabel->pixmap()->save(saveFileName, "PNG");
        QMessageBox::information(this, "Success", "Successfully saved thumbnail image");
    }
}

void ProfileEditor::replyFinishedBoxArt(QNetworkReply *aReply)
{
    QByteArray boxArt = aReply->readAll();
    if(boxArt.size() != 0 && !boxArt.contains("File not found."))
        ui->imgBoxArt->setPixmap(QPixmap::fromImage(QImage::fromData(boxArt)));
    else
        ui->imgBoxArt->setText("<i>Unable to download image.</i>");
}

void ProfileEditor::on_achievementsList_itemSelectionChanged()
{
    loadAchievementInfo(ui->gamesList->currentIndex().row(), ui->achievementsList->currentIndex().row());
}

void ProfileEditor::loadAchievementInfo(int gameIndex, unsigned int chievIndex)
{
    if (gameIndex < 0)
        return;
/*    if (chievIndex < 0)
        return;*/

    if (chievIndex >= games.at(gameIndex).gpd->achievements.size())
        return;

    AchievementEntry entry = games.at(gameIndex).gpd->achievements.at(chievIndex);
    ui->lblAchName->setText(QString::fromStdWString(entry.name).trimmed());
    ui->lblAchLockDesc->setText("Locked: <span style=\"color:#4f4f4f;\">" + QString::fromStdWString(entry.lockedDescription) + "</span>");
    ui->lblAchUnlDesc->setText("Unlocked: <span style=\"color:#4f4f4f;\">" + QString::fromStdWString(entry.unlockedDescription) + "</span>");
    ui->lblAchType->setText("Type: <span style=\"color:#4f4f4f;\">" + QString::fromStdString(GameGpd::GetAchievementType(&entry)) + "</span>");
    ui->lblAchGamescore->setText(QString::number(entry.gamerscore));

    if (entry.flags & UnlockedOnline)
    {
        ui->cmbxAchState->setCurrentIndex(2);
        ui->dteAchTimestamp->setEnabled(true);
    }
    else if (entry.flags & Unlocked)
    {
        ui->cmbxAchState->setCurrentIndex(1);
        ui->dteAchTimestamp->setEnabled(false);
    }
    else
    {
        ui->cmbxAchState->setCurrentIndex(0);
        ui->dteAchTimestamp->setEnabled(false);
    }

    ui->dteAchTimestamp->setDateTime(QDateTime::fromTime_t(entry.unlockTime).addMSecs(entry.unlockTimeMilliseconds));

    // set the thumbnail
    ImageEntry img;
    if (games.at(gameIndex).gpd->GetAchievementThumbnail(&entry, &img))
    {
        QByteArray imageBuff((char*)img.image, (size_t)img.length);
        ui->imgAch->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));
    }
    else
        ui->imgAch->setPixmap(QPixmap::fromImage(QImage(":/Images/HiddenAchievement.png")));
}

void ProfileEditor::on_aaGamelist_itemSelectionChanged()
{
    loadAwardGameInfo(ui->aaGamelist->currentIndex().row());
    loadAvatarAwardInfo(ui->aaGamelist->currentIndex().row(), 0);
}

void ProfileEditor::loadAwardGameInfo(int index)
{
    if (index < 0)
        return;

    ui->avatarAwardsList->clear();
    AvatarAwardGpd *gpd = aaGames.at(index).gpd;

    // load all the avatar awards for the game
    for (DWORD i = 0; i < gpd->avatarAwards.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->avatarAwardsList);
        item->setText(0, QString::fromStdWString(gpd->avatarAwards.at(i).name).replace("\n", ""));
        item->setText(1, QString::fromStdString(XdbfHelpers::AssetGenderToString(gpd->GetAssetGender(&gpd->avatarAwards.at(i)))));

        if (gpd->avatarAwards.at(i).flags & UnlockedOnline)
            item->setText(2, "Unlocked Online");
        else if (gpd->avatarAwards.at(i).flags & Unlocked)
            item->setText(2, "Unlocked Offline");
        else
            item->setText(2, "Locked");

        ui->avatarAwardsList->insertTopLevelItem(ui->avatarAwardsList->topLevelItemCount(), item);
    }

    ui->avatarAwardsList->setCurrentItem(ui->avatarAwardsList->topLevelItem(0));
    ui->avatarAwardsList->topLevelItem(0)->setSelected(true);

    // set the title information
    TitleEntry *title = aaGames.at(index).titleEntry;
    ui->lblAwGameName->setText("<span style=\"color:#4f4f4f;\">" + QString::fromStdWString(title->gameName) + "</span>");
    ui->lblAwGameTitleID->setText("<span style=\"color:#4f4f4f;\">" + QString::number(title->titleID, 16).toUpper() + "</span>");
    if (title->lastPlayed == 0x67D6Ca80)
        ui->lblAwGameLastPlayed->setText("<span style=\"color:#4f4f4f;\">N/A</span>");
    else
        ui->lblAwGameLastPlayed->setText("<span style=\"color:#4f4f4f;\">" + QDateTime::fromTime_t(title->lastPlayed).toString(Qt::SystemLocaleShortDate) + "</span>");
    ui->lblAwGameAwards->setText("<span style=\"color:#4f4f4f;\">" + QString::number(title->avatarAwardsEarned) + " out of " + QString::number(title->avatarAwardCount) + " unlocked" + "</span>");

    // download the box art image
    string tmp = DashboardGpd::GetSmallBoxArtURL(title);
    awardBoxArtManager->get(QNetworkRequest(QUrl(QString::fromStdString(tmp))));
}

void ProfileEditor::replyFinishedAwBoxArt(QNetworkReply *aReply)
{
    QByteArray boxArt = aReply->readAll();
    if(boxArt.size() != 0 && !boxArt.contains("File not found."))
    {
        ui->imgAwBoxArt->clear();
        ui->imgAwBoxArt->setPixmap(QPixmap::fromImage(QImage::fromData(boxArt)));
    }
    else
        ui->imgAwBoxArt->setText("<i>Unable to download image.</i>");
}

void ProfileEditor::on_avatarAwardsList_itemSelectionChanged()
{
    loadAvatarAwardInfo(ui->aaGamelist->currentIndex().row(), ui->avatarAwardsList->currentIndex().row());
}

void ProfileEditor::loadAvatarAwardInfo(int gameIndex, unsigned int awardIndex)
{
    if (gameIndex < 0)
        return;

    if (awardIndex >= aaGames.at(gameIndex).gpd->avatarAwards.size())
        return;

    // get the current award
    struct AvatarAward *award = &aaGames.at(gameIndex).gpd->avatarAwards.at(awardIndex);

    // update the ui
    ui->lblAwName->setText(QString::fromStdWString(award->name));
    ui->lblAwLockDesc->setText("Locked: <span style=\"color:#4f4f4f;\">" + QString::fromStdWString(award->lockedDescription) + "</span>");
    ui->lblAwUnlDesc->setText("Unlocked: <span style=\"color:#4f4f4f;\">" + QString::fromStdWString(award->unlockedDescription) + "</span>");

    try
    {
         ui->lblAwType->setText("Type: <span style=\"color:#4f4f4f;\">" + QString::fromStdString(XdbfHelpers::AssetSubcategoryToString(award->subcategory)) + "</span>");
    }
    catch (...)
    {
        ui->lblAwType->setText("Type: <span style=\"color:#4f4f4f;\"><i>Unknown</i></span>");
    }

    ui->lblAwGender->setText("Gender: <span style=\"color:#4f4f4f;\">" + QString::fromStdString(XdbfHelpers::AssetGenderToString(aaGames.at(gameIndex).gpd->GetAssetGender(award))) + "</span>");
    ui->lblAwSecret->setText(QString("Secret: <span style=\"color:#4f4f4f;\">") + ((award->flags & 0xFFFF) ? "No" : "Yes") + "</span>");

    if (award->flags & UnlockedOnline)
    {
        ui->cmbxAwState->setCurrentIndex(2);
        ui->dteAwTimestamp->setEnabled(true);
    }
    else if (award->flags & Unlocked)
    {
        ui->cmbxAwState->setCurrentIndex(1);
        ui->dteAwTimestamp->setEnabled(false);
    }
    else
    {
        ui->cmbxAwState->setCurrentIndex(0);
        ui->dteAwTimestamp->setEnabled(false);
    }

    ui->dteAwTimestamp->setDateTime(QDateTime::fromTime_t(award->unlockTime).addMSecs(award->unlockTimeMilliseconds));

    // download the thumbnail
    string tmp = AvatarAwardGpd::GetLargeAwardImageURL(award);
    QNetworkRequest request(QUrl(QString::fromStdString(tmp)));
    request.setAttribute((QNetworkRequest::Attribute)1001, QVariant::fromValue(award));
    awardThumbnailManager->get(request);
}

void ProfileEditor::replyFinishedAwImg(QNetworkReply *aReply)
{
    QByteArray img = aReply->readAll();
    if(img.size() != 0 && !img.contains("File not found."))
    {
        ui->imgAw->clear();
        ui->imgAw->setPixmap(QPixmap::fromImage(QImage::fromData(img)));

        struct AvatarAward *award = aReply->request().attribute((QNetworkRequest::Attribute)1001).value<struct AvatarAward*>();

        // add the avatar image to the gpd if it isn't already there
        if (ok && !ui->imgAw->pixmap()->isNull())
        {
            for (size_t i = 0; i < games.size(); i++)
            {
                if (games.at(i).titleEntry->titleID == award->titleID)
                {
                    GameGpd *gpd = games.at(i).gpd;
                    size_t x = 0;
                    for (; x < gpd->images.size(); x++)
                        if (gpd->images.at(x).entry.id == award->imageID)
                            break;
                    if (x == gpd->images.size())
                    {
                        ImageEntry image;
                        QByteArray ba;
                        QBuffer buffer(&ba);
                        buffer.open(QIODevice::WriteOnly);

                        ui->imgAw->pixmap()->scaled(64, 64).save(&buffer, "PNG");
                        image.image = new BYTE[ba.length()];
                        image.length = ba.length();
                        image.initialLength = ba.length();

                        memcpy(image.image, ba.data(), ba.length());

                        try
                        {
                            gpd->StartWriting();
                            gpd->CreateImageEntry(&image, award->imageID);
                            gpd->StopWriting();
                            games.at(i).updated = true;
                            break;
                        }
                        catch (string error)
                        {
                            QMessageBox::warning(this, "Save Error", "An error occurred while adding the avatar image to your profile.\n\n" + QString::fromStdString(error));
                            return;
                        }
                    }
                    else
                        break;
                }
            }
        }
    }
    else
        ui->imgAw->setText("<i>Unable to download image.</i>");
}

void ProfileEditor::on_btnUnlockAllAchvs_clicked()
{
    int index = ui->gamesList->currentIndex().row();
    if (index < 0)
        return;

    int btn = QMessageBox::question(this, "Are you sure?", "Are you sure that you want to unlock all of the achievements for " +
                          QString::fromStdWString(games.at(index).titleEntry->gameName) + " offline?", QMessageBox::Yes, QMessageBox::No);

    if (btn != QMessageBox::Yes)
        return;

    // update the ui
    for (int i = 0; i < ui->achievementsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->achievementsList->topLevelItem(i);
        if (item->text(1) != "Unlocked Online")
            item->setText(1, "Unlocked Offline");
    }

    unlockAllAchievements(index);

    ui->lblGameAchvs->setText("<span style=\"color:#4f4f4f;\">" + QString::number(games.at(index).titleEntry->achievementsUnlocked) + " out of " + QString::number(games.at(index).titleEntry->achievementCount) + " unlocked</span>");
    ui->lblGameGamerscore->setText("<span style=\"color:#4f4f4f;\">" + QString::number(games.at(index).titleEntry->gamerscoreUnlocked) + " out of " + QString::number(games.at(index).titleEntry->totalGamerscore) + " unlocked</span>");
    ui->lblGamerscore->setText(QString::number(dashGpd->gamerscoreUnlocked.int32) + "G");
}

void ProfileEditor::on_btnExtractGPD_clicked()
{
    int index = ui->gamesList->currentIndex().row();
    if (index < 0)
        return;

    QString gpdName = QString::number(games.at(index).titleEntry->titleID, 16).toUpper() + ".gpd";
    QString filePath = QFileDialog::getSaveFileName(this, "Choose a place to save the Gpd", QtHelpers::DesktopLocation() + "/" + gpdName, "Gpd File (*.gpd *.fit);;All Files (*.*)");

    if (filePath == "")
        return;

    try
    {
        profile->ExtractFile(gpdName.toStdString(), filePath.toStdString());
        statusBar->showMessage("Extracted Gpd successfully", 3000);
        QMessageBox::information(this, "Success", "Successfully extracted " + gpdName + " from your profile.\n");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "Failed to extract " + gpdName + ".\n\n" + QString::fromStdString(error));
    }
}

void ProfileEditor::on_btnExtractGPD_2_clicked()
{
    int index = ui->aaGamelist->currentIndex().row();
    if (index < 0)
        return;

    QString gpdName = QString::number(aaGames.at(index).titleEntry->titleID, 16).toUpper() + ".gpd";
    QString filePath = QFileDialog::getSaveFileName(this, "Choose a place to save the Gpd", QtHelpers::DesktopLocation() + "/" + gpdName, "Gpd File (*.gpd *.fit);;All Files (*.*)");

    if (filePath == "")
        return;

    try
    {
        PEC->ExtractFile(gpdName.toStdString(), filePath.toStdString());
        statusBar->showMessage("Extracted Gpd successfully", 3000);
        QMessageBox::information(this, "Success", "Successfully extracted " + gpdName + " from your profile.\n");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "Failed to extract " + gpdName + ".\n\n" + QString::fromStdString(error));
    }
}

void ProfileEditor::on_btnUnlockAllAwards_clicked()
{
    int index = ui->aaGamelist->currentIndex().row();
    if (index < 0)
        return;

    int btn = QMessageBox::question(this, "Are you sure?", "Are you sure that you want to unlock all of the avatar awards for " +
                          QString::fromStdWString(aaGames.at(index).titleEntry->gameName) + " offline?", QMessageBox::Yes, QMessageBox::No);

    if (btn != QMessageBox::Yes)
      return;

    unlockAllAwards(index);

    // update the ui
    for (int i = 0; i < ui->avatarAwardsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->avatarAwardsList->topLevelItem(i);
        if (item->text(2) != "Unlocked Online")
            item->setText(2, "Unlocked Offline");
    }

    ui->lblAwGameAwards->setText("<span style=\"color:#4f4f4f;\">" + QString::number(aaGames.at(index).titleEntry->avatarAwardsEarned) + " out of " + QString::number(aaGames.at(index).titleEntry->avatarAwardCount) + " unlocked</span>");
    statusBar->showMessage("All awards unlocked for " + QString::fromStdWString(aaGames.at(index).titleEntry->gameName), 3000);
}

void ProfileEditor::updateAvatarAward(TitleEntry *entry, AvatarAwardGpd *gpd, struct AvatarAward *award, State toSet)
{
    AssetGender g = AvatarAwardGpd::GetAssetGender(award);
    State current = getStateFromFlags(award->flags);

    // update title entry
    if (current == StateLocked)
    {
        entry->avatarAwardsEarned++;

        if (g == Male)
            entry->maleAvatarAwardsEarned++;
        else if (g == Female)
            entry->femaleAvatarAwardsEarned++;
    }
    else if (toSet == StateLocked)
    {
        entry->avatarAwardsEarned--;

        if (g == Male)
            entry->maleAvatarAwardsEarned--;
        else if (g == Female)
            entry->femaleAvatarAwardsEarned--;

        award->flags &= 0xFFFCFFFF;
    }

    if (toSet == StateUnlockedOffline)
    {
        award->flags &= 0xFFFCFFFF;
        award->flags |= (Unlocked | 0x100000);

        if (award->subcategory == 0)
            award->subcategory = (AssetSubcategory)0xFFFFFFFF;
        award->colorizable = 0;
    }
    else if (toSet == StateUnlockedOnline)
    {
        award->flags |= (Unlocked | UnlockedOnline | 0x100000);
        if (award->subcategory == 0)
            award->subcategory = (AssetSubcategory)0xFFFFFFFF;
        award->colorizable = 0;
        entry->lastPlayed = QDateTime::currentDateTime().toTime_t();
    }

    // Write the entry back to the gpd
    gpd->WriteAvatarAward(award);

    entry->flags |= (DownloadAvatarAward | SyncAvatarAward);

    // update the dash gpd
    dashGpd->WriteTitleEntry(entry);

    statusBar->showMessage("Updated " + QString::fromStdWString(award->name), 3000);
}

void ProfileEditor::updateAchievement(TitleEntry *entry, AchievementEntry *chiev, State toSet, GameGpd *gpd)
{
    try
    {
        gpd->StartWriting();

        // make sure that the settings exist
        SettingEntry settingGamerscore, settingAchievements;
        settingGamerscore.int32 = settingAchievements.int32 = 0xFFFFFFFF;
        for (DWORD i = 0; i < gpd->settings.size(); i++)
            if (gpd->settings.at(i).entry.id == GamercardTitleAchievementsEarned)
                settingAchievements = gpd->settings.at(i);
            else if (gpd->settings.at(i).entry.id == GamercardTitleCredEarned)
                settingGamerscore = gpd->settings.at(i);

        // if the settings don't exist, then create them
        if (settingGamerscore.int32 == 0xFFFFFFFF)
        {
            settingGamerscore.type = Int32;
            settingGamerscore.int32 = 0;
            gpd->CreateSettingEntry(&settingGamerscore, GamercardTitleCredEarned);
        }
        if (settingAchievements.int32 == 0xFFFFFFFF)
        {
            settingAchievements.type = Int32;
            settingAchievements.int32 = 0;
            gpd->CreateSettingEntry(&settingAchievements, GamercardTitleAchievementsEarned);
        }

        State current = getStateFromFlags(chiev->flags);

        if (current == StateLocked)
        {
            if (toSet == StateUnlockedOnline)
                entry->achievementsUnlockedOnline++;

            // update title entry
            entry->achievementsUnlocked++;
            entry->gamerscoreUnlocked += chiev->gamerscore;

            // update dash gpd, overall values
            dashGpd->achievementsUnlocked.int32++;
            dashGpd->gamerscoreUnlocked.int32 += chiev->gamerscore;

            dashGpd->WriteSettingEntry(dashGpd->achievementsUnlocked);
            dashGpd->WriteSettingEntry(dashGpd->gamerscoreUnlocked);

            // update game gpd
            settingAchievements.int32++;
            settingGamerscore.int32 += chiev->gamerscore;

            gpd->WriteSettingEntry(settingAchievements);
            gpd->WriteSettingEntry(settingGamerscore);
        }
        else if (toSet == StateLocked)
        {
            entry->achievementsUnlocked--;
            if (current == StateUnlockedOnline)
                entry->achievementsUnlockedOnline--;

            entry->gamerscoreUnlocked -= chiev->gamerscore;

            dashGpd->achievementsUnlocked.int32--;
            dashGpd->gamerscoreUnlocked.int32 -= chiev->gamerscore;

            dashGpd->WriteSettingEntry(dashGpd->achievementsUnlocked);
            dashGpd->WriteSettingEntry(dashGpd->gamerscoreUnlocked);

            settingAchievements.int32--;
            settingGamerscore.int32 -= chiev->gamerscore;

            gpd->WriteSettingEntry(settingAchievements);
            gpd->WriteSettingEntry(settingGamerscore);

            chiev->flags &= 0xFFFCFFFF;
        }

        if (toSet == StateUnlockedOffline)
        {
            chiev->flags &= 0xFFFCFFFF;
            chiev->flags |= (Unlocked | 0x100000);
        }
        else if (toSet == StateUnlockedOnline)
        {
            chiev->flags |= (Unlocked | UnlockedOnline | 0x100000);
            entry->lastPlayed = QDateTime::currentDateTime().toTime_t();
        }

        entry->flags |= (SyncAchievement | DownloadAchievementImage);

        gpd->WriteAchievementEntry(chiev);

        dashGpd->WriteTitleEntry(entry);

        ui->lblGamerscore->setText(QString::number(dashGpd->gamerscoreUnlocked.int32) + "G");

        statusBar->showMessage("Updated " + QString::fromStdWString(chiev->name), 3000);

        gpd->StopWriting();
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while updating the achievement.\n\n" + QString::fromStdString(error));
    }
}

void ProfileEditor::saveAll()
{
    // validate the bio length
    if (ui->txtBio->toPlainText().length() > 499)
    {
        QMessageBox::warning(this, "Invalid Length", "The length of a gamer's bio must be less than or equal to 499 characters.\n");
        return;
    }

    string path = QtHelpers::GetKVPath(profile->metaData->certificate.ownerConsoleType, this);
    
    // save the avatar awards
    if (PEC != NULL)
    {
        // put all the avatar award gpds back in the PEC
        for (DWORD i = 0; i < aaGames.size(); i++)
            if (aaGames.at(i).updated)
                PEC->ReplaceFile(aaGames.at(i).tempFileName, aaGames.at(i).gpdName);

        PEC->metaData->WriteMetaData();
        PEC->Rehash();

        if (path != "")
            PEC->Resign(path);

        delete PEC;

        //profile->ReplaceFile(pecTempPath, "PEC");
    }

    // save the achievements
    for (DWORD i = 0; i < games.size(); i++)
        if (games.at(i).updated)
            profile->ReplaceFile(games.at(i).tempFileName, games.at(i).gpdName);


    // save all of the stuff on the front page

    // gamertag
    account->SetGamertag(ui->txtGamertag->text().toStdWString());
    account->SetPasscodeEnabled(ui->chxPasscode->checkState() >> 1);
    account->SetLiveEnabled(ui->chxLIVE->checkState() >> 1);
    account->SetRecovering(ui->chxRecovering->checkState() >> 1);

    BYTE passcode[4] = { (BYTE)ui->cmbxPass1->currentIndex(), (BYTE)ui->cmbxPass2->currentIndex(), (BYTE)ui->cmbxPass3->currentIndex(), (BYTE)ui->cmbxPass4->currentIndex() };
    account->SetPasscode(passcode);

    switch (ui->cmbxNetwork->currentIndex())
    {
        case 0:
            account->SetOnlineServiceProvider(ProductionNet);
            break;
        case 1:
            account->SetOnlineServiceProvider(PartnerNet);
            break;
    }

    account->Save(profile->metaData->certificate.ownerConsoleType);
    profile->ReplaceFile(accountTempPath, "Account");

    // gamer name
    wstring temp = ui->txtName->text().toStdWString();
    dashGpd->gamerName.str = &temp;
    dashGpd->WriteSettingEntry(dashGpd->gamerName);

    // motto
    temp = ui->txtMotto->text().toStdWString();
    dashGpd->motto.str = &temp;
    dashGpd->WriteSettingEntry(dashGpd->motto);

    // location
    temp = ui->txtLocation->text().toStdWString();
    dashGpd->gamerLocation.str = &temp;
    dashGpd->WriteSettingEntry(dashGpd->gamerLocation);

    // gamerzone
    dashGpd->gamerzone.int32 = ui->cmbxGamerzone->currentIndex();
    dashGpd->WriteSettingEntry(dashGpd->gamerzone);

    // region
    dashGpd->gamercardRegion.int32 = regions[ui->cmbxRegion->currentIndex()].value;
    dashGpd->WriteSettingEntry(dashGpd->gamercardRegion);

    // years on LIVE
    dashGpd->yearsOnLive.int32 = ui->cmbxTenure->currentIndex();
    dashGpd->WriteSettingEntry(dashGpd->yearsOnLive);

    // reputation
    dashGpd->reputation.floatData = (float)ui->spnRep->value();
    dashGpd->WriteSettingEntry(dashGpd->reputation);

    // bio
    temp = ui->txtBio->toPlainText().toStdWString();
    dashGpd->gamerBio.str = &temp;
    dashGpd->WriteSettingEntry(dashGpd->gamerBio);

    // put the dash gpd back in the profile
    //profile->ReplaceFile(dashGpdTempPath, "FFFE07D1.gpd");

    // fix the package
    profile->metaData->WriteMetaData();
    profile->Rehash();
    if (path != "")
        profile->Resign(path);
}

State ProfileEditor::getStateFromFlags(DWORD flags)
{
    if (flags & UnlockedOnline)
        return StateUnlockedOnline;
    else if (flags & Unlocked)
        return StateUnlockedOffline;
    else
        return StateLocked;
}

void ProfileEditor::on_btnCreateAch_clicked()
{
    if (ui->gamesList->currentIndex().row() < 0)
        return;

    AchievementEntry entry;
    QImage thumbnail;
    AchievementCreationWizard wiz(games.at(ui->gamesList->currentIndex().row()).gpd->gameName.ws, &thumbnail, &entry, this);
    wiz.exec();

    // make sure the user didn't exit out of the wizard early
    if (entry.structSize == 0x1C)
    {
        GameGpd *game = games.at(ui->gamesList->currentIndex().row()).gpd;
        games.at(ui->gamesList->currentIndex().row()).updated = true;

        // convert the image data to PNG and get a pointer to the data
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        QPixmap::fromImage(thumbnail).save(&buffer, "PNG");

        // get the next available image id
        DWORD max = 0;
        for (size_t i = 0; i < game->achievements.size(); i++)
            if (max < game->achievements.at(i).imageID)
                max = game->achievements.at(i).imageID;
        entry.imageID = max + 1;

        // add the achievement to the game
        try
        {
            game->CreateAchievement(&entry, (BYTE*)ba.data(), ba.length());
        }
        catch (string error)
        {
            QMessageBox::critical(this, "Creation Error", "An error occurred while creating an achievement.\n\n" + QString::fromStdString(error));
        }

        // add the achievement to the UI
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->achievementsList);
        item->setText(0, QString::fromStdWString(entry.name));
        item->setText(1, "Locked");
        item->setText(2, QString::number(entry.gamerscore));
        ui->achievementsList->insertTopLevelItem(ui->achievementsList->topLevelItemCount(), item);

        // update the title entry
        TitleEntry *title = games.at(ui->gamesList->currentIndex().row()).titleEntry;
        title->achievementCount++;
        title->totalGamerscore += entry.gamerscore;
        dashGpd->WriteTitleEntry(title);
    }
}

void ProfileEditor::on_cmbxAchState_currentIndexChanged(int index)
{
    int gameIndex = ui->gamesList->currentIndex().row();
    int chievIndex = ui->achievementsList->currentIndex().row();

    if (gameIndex < 0)
        return;
    if (chievIndex < 0)
        return;

    GameGpd *gpd = games.at(gameIndex).gpd;

    // make sure that the user changed the value, and they didn't just select another achievement
    if (index == getStateFromFlags(gpd->achievements.at(chievIndex).flags))
        return;

    // update the achievement entry
    if (index == 0)
    {
        // update the list text
        ui->achievementsList->topLevelItem(chievIndex)->setText(1, "Locked");

        // update the title entry
        updateAchievement(games.at(gameIndex).titleEntry, &gpd->achievements.at(chievIndex), StateLocked, gpd);

        ui->dteAchTimestamp->setEnabled(false);
    }
    else if (index == 1)
    {
        // update the list text
        ui->achievementsList->topLevelItem(chievIndex)->setText(1, "Unlocked Offline");

        // update the title entry
        updateAchievement(games.at(gameIndex).titleEntry, &gpd->achievements.at(chievIndex), StateUnlockedOffline, gpd);

        ui->lblGameAchvs->setText("<span style=\"color:#4f4f4f;\">" + QString::number(games.at(gameIndex).titleEntry->achievementsUnlocked) + " out of " + QString::number(games.at(gameIndex).titleEntry->achievementCount) + " unlocked</span>");
        ui->lblGameGamerscore->setText("<span style=\"color:#4f4f4f;\">Gamerscore: " + QString::number(games.at(gameIndex).titleEntry->gamerscoreUnlocked) + " out of " + QString::number(games.at(gameIndex).titleEntry->totalGamerscore) + " unlocked</span>");

        ui->dteAchTimestamp->setEnabled(false);
    }
    else
    {
        // update the list text
        ui->achievementsList->topLevelItem(chievIndex)->setText(1, "Unlocked Online");

        // update the title entry
        updateAchievement(games.at(gameIndex).titleEntry, &gpd->achievements.at(chievIndex), StateUnlockedOnline, gpd);

        ui->lblGameAchvs->setText("<span style=\"color:#4f4f4f;\">Achievements: " + QString::number(games.at(gameIndex).titleEntry->achievementsUnlocked) + " out of " + QString::number(games.at(gameIndex).titleEntry->achievementCount) + " unlocked</span>");
        ui->lblGameGamerscore->setText("<span style=\"color:#4f4f4f;\">Gamerscore: " + QString::number(games.at(gameIndex).titleEntry->gamerscoreUnlocked) + " out of " + QString::number(games.at(gameIndex).titleEntry->totalGamerscore) + " unlocked</span>");

        ui->dteAchTimestamp->setEnabled(true);
        ui->dteAchTimestamp->setDateTime(QDateTime::currentDateTime());
    }

    games.at(gameIndex).updated = true;
}

void ProfileEditor::on_cmbxAwState_currentIndexChanged(int index)
{
    int gameIndex = ui->aaGamelist->currentIndex().row();
    int awardIndex = ui->avatarAwardsList->currentIndex().row();

    if (gameIndex < 0)
        return;
    if (awardIndex < 0)
        return;

    AvatarAwardGpd *gpd = aaGames.at(gameIndex).gpd;

    // make sure that the user changed the value, and they didn't just select another achievement
    if (index == getStateFromFlags(gpd->avatarAwards.at(awardIndex).flags))
        return;

    // update the award entry
    if (index == 0)
    {
        // update the list text
        ui->avatarAwardsList->topLevelItem(awardIndex)->setText(2, "Locked");

        // update the title entry
        updateAvatarAward(aaGames.at(gameIndex).titleEntry, gpd, &gpd->avatarAwards.at(awardIndex), StateLocked);
        ui->dteAwTimestamp->setEnabled(false);
    }
    else if (index == 1)
    {
        // update the list text
        ui->avatarAwardsList->topLevelItem(awardIndex)->setText(2, "Unlocked Offline");

        // update the title entry
        updateAvatarAward(aaGames.at(gameIndex).titleEntry, gpd, &gpd->avatarAwards.at(awardIndex), StateUnlockedOffline);
        ui->dteAwTimestamp->setEnabled(false);

        ui->lblAwGameAwards->setText("<span style=\"color:#4f4f4f;\">Awards: " + QString::number(aaGames.at(gameIndex).titleEntry->avatarAwardsEarned) + " out of " + QString::number(aaGames.at(gameIndex).titleEntry->avatarAwardCount) + " unlocked</span>");
    }
    else
    {
        // update the list text
        ui->avatarAwardsList->topLevelItem(awardIndex)->setText(2, "Unlocked Online");

        // update the title entry
        updateAvatarAward(aaGames.at(gameIndex).titleEntry, gpd, &gpd->avatarAwards.at(awardIndex), StateUnlockedOnline);

        ui->lblAwGameAwards->setText("<span style=\"color:#4f4f4f;\">Awards: " + QString::number(aaGames.at(gameIndex).titleEntry->avatarAwardsEarned) + " out of " + QString::number(aaGames.at(gameIndex).titleEntry->avatarAwardCount) + " unlocked</span>");

        ui->dteAwTimestamp->setEnabled(true);
        ui->dteAwTimestamp->setDateTime(QDateTime::currentDateTime());
    }

    aaGames.at(gameIndex).updated = true;
}

void ProfileEditor::on_btnShowAll_clicked()
{
    ui->txtGameSearch->setText("");
    showAllGames();
}

void ProfileEditor::showAllGames()
{
    // show all the items
    for (int i = 0; i < ui->gamesList->topLevelItemCount(); i++)
        ui->gamesList->setItemHidden(ui->gamesList->topLevelItem(i), false);
}

void ProfileEditor::showAllAwardGames()
{
    // show all the items
    for (int i = 0; i < ui->aaGamelist->topLevelItemCount(); i++)
        ui->aaGamelist->setItemHidden(ui->aaGamelist->topLevelItem(i), false);
}

void ProfileEditor::on_btnAwardShowAll_clicked()
{
    ui->txtAwardGameSearch->setText("");
    showAllAwardGames();
}

void ProfileEditor::on_chxPasscode_stateChanged(int arg1)
{
    ui->cmbxPass1->setEnabled(arg1 >> 1);
    ui->cmbxPass2->setEnabled(arg1 >> 1);
    ui->cmbxPass3->setEnabled(arg1 >> 1);
    ui->cmbxPass4->setEnabled(arg1 >> 1);

    if (arg1 == 0)
    {
        ui->cmbxPass1->setCurrentIndex(0);
        ui->cmbxPass2->setCurrentIndex(0);
        ui->cmbxPass3->setCurrentIndex(0);
        ui->cmbxPass4->setCurrentIndex(0);
    }
}

void ProfileEditor::on_chxLIVE_stateChanged(int arg1)
{
    ui->cmbxNetwork->setEnabled(arg1 >> 1);

    if (arg1 == 0)
        account->SetXUIDOffline();
    else
        account->SetXUIDOnline();
    ui->lblXUID->setText(QString::number(account->GetXUID(), 16).toUpper());
}

void ProfileEditor::on_dteAchTimestamp_dateTimeChanged(const QDateTime &date)
{
    if (ui->gamesList->currentIndex().row() < 0 || ui->achievementsList->currentIndex().row() < 0)
        return;

    AchievementEntry *entry = &games.at(ui->gamesList->currentIndex().row()).gpd->achievements.at(ui->achievementsList->currentIndex().row());

    // make sure the user changed the time
    if (date.toTime_t() == entry->unlockTime)
        return;

    games.at(ui->gamesList->currentIndex().row()).gpd->StartWriting();

    entry->unlockTime = date.toTime_t();
    entry->unlockTimeMilliseconds = date.time().msec();

    ui->dteAchTimestamp->setDateTime(QDateTime::fromTime_t(entry->unlockTime).addMSecs(entry->unlockTimeMilliseconds));
    games.at(ui->gamesList->currentIndex().row()).gpd->WriteAchievementEntry(entry);
    games.at(ui->gamesList->currentIndex().row()).updated = true;

    games.at(ui->gamesList->currentIndex().row()).gpd->StopWriting();

    statusBar->showMessage("Updated " + QString::fromStdWString(entry->name), 3000);
}

void ProfileEditor::on_dteAwTimestamp_dateTimeChanged(const QDateTime &date)
{
    if (ui->aaGamelist->currentIndex().row() < 0 || ui->avatarAwardsList->currentIndex().row() < 0)
        return;

    struct AvatarAward *entry = &aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row());

    // make sure the user changed the time
    if (date.toTime_t() == entry->unlockTime)
        return;

    entry->unlockTime = date.toTime_t();
    entry->unlockTimeMilliseconds = date.time().msec();
    aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->WriteAvatarAward(entry);
    aaGames.at(ui->aaGamelist->currentIndex().row()).updated = true;

   statusBar->showMessage("Updated " + QString::fromStdWString(entry->name), 3000);
}

void ProfileEditor::on_txtGameSearch_textChanged(const QString & /*arg1*/)
{
    QList<QTreeWidgetItem*> itemsMatched = ui->gamesList->findItems(ui->txtGameSearch->text(), Qt::MatchContains);

    // hide all the items
    for (int i = 0; i < ui->gamesList->topLevelItemCount(); i++)
        ui->gamesList->setItemHidden(ui->gamesList->topLevelItem(i), true);

    if (itemsMatched.count() == 0)
    {
        ui->txtGameSearch->setStyleSheet("color: rgb(255, 1, 1);");
        return;
    }

    ui->txtGameSearch->setStyleSheet("");
    // add all the matched ones to the list
    for (int i = 0; i < itemsMatched.count(); i++)
        ui->gamesList->setItemHidden(itemsMatched.at(i), false);
}

void ProfileEditor::on_txtAwardGameSearch_textChanged(const QString & /* arg1 */)
{
    QList<QTreeWidgetItem*> itemsMatched = ui->aaGamelist->findItems(ui->txtAwardGameSearch->text(), Qt::MatchContains);

    // hide all the items
    for (int i = 0; i < ui->aaGamelist->topLevelItemCount(); i++)
        ui->aaGamelist->setItemHidden(ui->aaGamelist->topLevelItem(i), true);

    if (itemsMatched.count() == 0)
    {
        ui->txtAwardGameSearch->setStyleSheet("color: rgb(255, 1, 1);");
        return;
    }

    ui->txtAwardGameSearch->setStyleSheet("");
    // add all the matched ones to the list
    for (int i = 0; i < itemsMatched.count(); i++)
        ui->aaGamelist->setItemHidden(itemsMatched.at(i), false);
}

void ProfileEditor::on_tabWidget_currentChanged(int index)
{
    unlockEverything->setHidden(index == 0);
    unlockEverything->setEnabled(((index == 1) ? (ui->gamesList->topLevelItemCount() != 0) : (ui->aaGamelist->topLevelItemCount() != 0)));
}

void ProfileEditor::onUnlockEverything()
{
    QMessageBox::StandardButton btn = (QMessageBox::StandardButton)QMessageBox::question(this, "Continue?", "Are you sure that you want to unlock all achievements and avatar awards offline?", QMessageBox::Yes, QMessageBox::No);
    if (btn != QMessageBox::Yes)
        return;

    QString max =  QString::number(ui->gamesList->topLevelItemCount() + ui->aaGamelist->topLevelItemCount());
    statusBar->showMessage("Unlocking... 0/" + max);
    ui->tabWidget->setEnabled(false);

    DWORD totalGamerscore = 0;

    // unlock all achievements
    for (int i = 0; i < ui->gamesList->topLevelItemCount(); i++)
    {
        unlockAllAchievements(i);
        totalGamerscore += games.at(i).titleEntry->totalGamerscore;

        statusBar->showMessage("Unlocking... " + QString::number(i + 1) + "/" + max);
        QApplication::processEvents();
    }
    if (ui->gamesList->topLevelItemCount() != 0)
    {
        ui->gamesList->setCurrentItem(ui->gamesList->topLevelItem(0));
        ui->gamesList->topLevelItem(0)->setSelected(true);
        loadGameInfo(ui->gamesList->currentIndex().row());
        loadAchievementInfo(ui->gamesList->currentIndex().row(), 0);
    }

    // update the ui
    ui->lblGamerscore->setText(QString::number(totalGamerscore) + "G");

    // unlock all awards
    for (int i = 0; i < ui->aaGamelist->topLevelItemCount(); i++)
    {
        unlockAllAwards(i);

        statusBar->showMessage("Unlocking... " + QString::number(i + ui->gamesList->topLevelItemCount() + 1) + "/" + max);
        QApplication::processEvents();
    }
    if (ui->aaGamelist->topLevelItemCount() != 0)
    {
        ui->aaGamelist->setCurrentItem(ui->aaGamelist->topLevelItem(0));
        ui->aaGamelist->topLevelItem(0)->setSelected(true);
        loadAwardGameInfo(ui->aaGamelist->currentIndex().row());
        loadAvatarAwardInfo(ui->aaGamelist->currentIndex().row(), 0);
    }

    statusBar->showMessage("Successfully unlocked everything", 3000);
    ui->tabWidget->setEnabled(true);
}

void ProfileEditor::unlockAllAchievements(int index)
{
    DWORD gamerscoreToAdd = 0;
    for (DWORD i = 0; i < games.at(index).gpd->achievements.size(); i++)
        if ((games.at(index).gpd->achievements.at(i).flags & Unlocked) == 0)
            gamerscoreToAdd += games.at(index).gpd->achievements.at(i).gamerscore;

    games.at(index).gpd->StartWriting();
    games.at(index).gpd->UnlockAllAchievementsOffline();
    games.at(index).updated = true;

    // make sure that the settings exist
    SettingEntry settingGamerscore, settingAchievements;
    settingGamerscore.int32 = settingAchievements.int32 = 0xFFFFFFFF;
    for (DWORD i = 0; i < games.at(index).gpd->settings.size(); i++)
        if (games.at(index).gpd->settings.at(i).entry.id == GamercardTitleAchievementsEarned)
            settingAchievements = games.at(index).gpd->settings.at(i);
        else if (games.at(index).gpd->settings.at(i).entry.id == GamercardTitleCredEarned)
            settingGamerscore = games.at(index).gpd->settings.at(i);

    // if the settings don't exist, then create them
    if (settingGamerscore.int32 == 0xFFFFFFFF)
    {
        settingGamerscore.type = Int32;
        settingGamerscore.int32 = 0;
        games.at(index).gpd->CreateSettingEntry(&settingGamerscore, GamercardTitleCredEarned);
    }
    if (settingAchievements.int32 == 0xFFFFFFFF)
    {
        settingAchievements.type = Int32;
        settingAchievements.int32 = 0;
        games.at(index).gpd->CreateSettingEntry(&settingAchievements, GamercardTitleAchievementsEarned);
    }

    // set the settings to the max amounts
    settingGamerscore.int32 = games.at(index).titleEntry->totalGamerscore;
    settingAchievements.int32 = games.at(index).titleEntry->achievementCount;

    // Write the settings
    games.at(index).gpd->WriteSettingEntry(settingGamerscore);
    games.at(index).gpd->WriteSettingEntry(settingAchievements);

    games.at(index).titleEntry->achievementsUnlocked = games.at(index).titleEntry->achievementCount;
    games.at(index).titleEntry->gamerscoreUnlocked = games.at(index).titleEntry->totalGamerscore;
    games.at(index).titleEntry->flags |= (SyncAchievement | DownloadAchievementImage);

    games.at(index).gpd->StopWriting();

    dashGpd->WriteTitleEntry(games.at(index).titleEntry);

    dashGpd->gamerscoreUnlocked.int32 += gamerscoreToAdd;
    dashGpd->WriteSettingEntry(dashGpd->gamerscoreUnlocked);
}

void ProfileEditor::unlockAllAwards(int index)
{
    aaGames.at(index).gpd->UnlockAllAwards();
    aaGames.at(index).updated = true;

    // update the title entry
    aaGames.at(index).titleEntry->avatarAwardsEarned = aaGames.at(index).titleEntry->avatarAwardCount;
    aaGames.at(index).titleEntry->maleAvatarAwardsEarned = aaGames.at(index).titleEntry->maleAvatarAwardCount;
    aaGames.at(index).titleEntry->femaleAvatarAwardsEarned = aaGames.at(index).titleEntry->femaleAvatarAwardCount;
    aaGames.at(index).titleEntry->flags |= (DownloadAvatarAward | SyncAvatarAward);

    dashGpd->WriteTitleEntry(aaGames.at(index).titleEntry);
}

void ProfileEditor::on_cmbxConsoleType_currentIndexChanged(int index)
{
    if (index == 0)
    {
        profile->metaData->certificate.ownerConsoleType = DevKit;
        if (PEC)
            PEC->metaData->certificate.ownerConsoleType = DevKit;
    }
    else
    {
        profile->metaData->certificate.ownerConsoleType = Retail;
        profile->metaData->certificate.consoleTypeFlags = (ConsoleTypeFlags)0;
        if (PEC)
        {
            PEC->metaData->certificate.ownerConsoleType = Retail;
            PEC->metaData->certificate.consoleTypeFlags = (ConsoleTypeFlags)0;
        }

    }
}

bool ProfileEditor::isOk()
{
    return ok;
}