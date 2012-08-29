#include "profileeditor.h"
#include "ui_profileeditor.h"

ProfileEditor::ProfileEditor(StfsPackage *profile, bool dispose, QWidget *parent) : QDialog(parent), ui(new Ui::ProfileEditor), profile(profile), dispose(dispose), PEC(NULL)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->achievementsList->header()->resizeSection(0, 150);
    ui->achievementsList->header()->resizeSection(1, 235);

    ui->avatarAwardsList->header()->resizeSection(0, 150);
    ui->avatarAwardsList->header()->resizeSection(1, 235);

    // verify that the package is a profile
    if (profile->metaData->contentType != Profile)
    {
        QMessageBox::critical(this, "Invalid Package", "Package opened isn't a profile.\n");
        this->close();
        return;
    }

    // make sure the dashboard gpd exists
    if (!profile->FileExists("FFFE07D1.gpd"))
    {
        QMessageBox::critical(this, "File Not Found", "Dashboard GPD, FFFE07D1.gpd, wasn't found.\n");
        this->close();
        return;
    }

    ///////////////////////////////////
    // LOAD FRONT PAGE
    ///////////////////////////////////

    // populate the years on live combo box
    for (DWORD i = 0; i <= 20; i++)
        ui->cmbxTenure->addItem(QString::number(i));

    // populate the regions combo box
    for (DWORD i = 0; i < 109; i++)
        ui->cmbxRegion->addItem(regions[i].name);

    // extract the dashboard gpd
    dashGPDTempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
    tempFiles.push_back(dashGPDTempPath);
    profile->ExtractFile("FFFE07D1.gpd", dashGPDTempPath);

    // parse the dashboard gpd
    dashGPD = new DashboardGPD(dashGPDTempPath);

    // load all the goodies
    if (dashGPD->gamerName.entry.type == 0)
        addToQueue(UnicodeString, GamercardUserName);
    else
        ui->txtName->setText(QString::fromStdWString(*dashGPD->gamerName.str));

    if (dashGPD->motto.entry.type == 0)
        addToQueue(UnicodeString, GamercardMotto);
    else
        ui->txtMotto->setText(QString::fromStdWString(*dashGPD->motto.str));

    if (dashGPD->gamerLocation.entry.type == 0)
        addToQueue(UnicodeString, GamercardUserLocation);
    else
        ui->txtLocation->setText(QString::fromStdWString(*dashGPD->gamerLocation.str));

    if (dashGPD->gamerzone.entry.type == 0)
        addToQueue(Int32, GamercardZone);
    else
        ui->cmbxGamerzone->setCurrentIndex(dashGPD->gamerzone.int32);

    if (dashGPD->gamercardRegion.entry.type == 0)
        addToQueue(Int32, GamercardRegion);
    else
    {
        // find the index of the gamer's region
        for (DWORD i = 0; i < 109; i++)
           if (regions[i].value == dashGPD->gamercardRegion.int32)
               ui->cmbxRegion->setCurrentIndex(i);
    }

    if (dashGPD->yearsOnLive.entry.type == 0)
        addToQueue(Int32, YearsOnLive);
    else
        ui->cmbxTenure->setCurrentIndex(dashGPD->yearsOnLive.int32);

    if (dashGPD->reputation.entry.type == 0)
        addToQueue(Float, GamercardRep);
    else
        ui->spnRep->setValue(dashGPD->reputation.floatData);

    if (dashGPD->gamerBio.entry.type == 0)
        addToQueue(UnicodeString, GamercardUserBio);
    else
        ui->txtBio->setPlainText(QString::fromStdWString(*dashGPD->gamerBio.str));


    // load the avatar image
    if (dashGPD->avatarImage.entry.type != 0)
    {
        QByteArray imageBuff((char*)dashGPD->avatarImage.image, (size_t)dashGPD->avatarImage.length);
        ui->imgAvatar->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));
    }
    else
        ui->imgAvatar->setPixmap(QPixmap::fromImage(QImage(":/Images/avatar-body.png")));

    // load the avatar colors
    if (dashGPD->avatarInformation.entry.type == 0)
    {
        ui->gbAvatarColors->setTitle("Avatar Colors - Not Found");
        ui->gbAvatarColors->setEnabled(false);
    }
    else
    {
        BYTE *colors = &dashGPD->avatarInformation.binaryData.data[0xFC];
        ui->clrSkin->setStyleSheet("* { background-color: rgb(" + QString::number(colors[1]) + "," + QString::number(colors[2]) + "," + QString::number(colors[3]) + ") }");
        ui->clrHair->setStyleSheet("* { background-color: rgb(" + QString::number(colors[5]) + "," + QString::number(colors[6]) + "," + QString::number(colors[7]) + ") }");
        ui->clrLips->setStyleSheet("* { background-color: rgb(" + QString::number(colors[9]) + "," + QString::number(colors[10]) + "," + QString::number(colors[11]) + ") }");
        ui->clrEyes->setStyleSheet("* { background-color: rgb(" + QString::number(colors[13]) + "," + QString::number(colors[14]) + "," + QString::number(colors[15]) + ") }");
        ui->clrEyebrows->setStyleSheet("* { background-color: rgb(" + QString::number(colors[17]) + "," + QString::number(colors[18]) + "," + QString::number(colors[19]) + ") }");
        ui->clrEyeShadow->setStyleSheet("* { background-color: rgb(" + QString::number(colors[21]) + "," + QString::number(colors[22]) + "," + QString::number(colors[23]) + ") }");
        ui->clrFacialHair->setStyleSheet("* { background-color: rgb(" + QString::number(colors[25]) + "," + QString::number(colors[24]) + "," + QString::number(colors[27]) + ") }");
        ui->clrFacePaint->setStyleSheet("* { background-color: rgb(" + QString::number(colors[29]) + "," + QString::number(colors[28]) + "," + QString::number(colors[31]) + ") }");
        ui->clrfacePaint2->setStyleSheet("* { background-color: rgb(" + QString::number(colors[33]) + "," + QString::number(colors[30]) + "," + QString::number(colors[35]) + ") }");;
    }

    ////////////////////////////
    // LOAD ACHIEVEMENTS
    ////////////////////////////

    // load all the games played
    for (DWORD i = 0; i < dashGPD->gamesPlayed.size(); i++)
    {
        // if the game doesn't have any achievements, no need to load it
        if (dashGPD->gamesPlayed.at(i).achievementCount == 0)
            continue;

        // make sure the corresponding gpd exists
        QString gpdName = QString::number(dashGPD->gamesPlayed.at(i).titleID, 16).toUpper() + ".gpd";
        if (!profile->FileExists(gpdName.toStdString()))
        {
            QMessageBox::critical(this, "File Not Found", "Couldn't find file \"" + gpdName + "\".");
            this->close();
        }

        // extract the gpd
        string tempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        profile->ExtractFile(gpdName.toStdString(), tempPath);
        tempFiles.push_back(tempPath);

        // parse the gpd
        GameGPD *gpd;
        try
        {
            gpd = new GameGPD(tempPath);
        }
        catch (string error)
        {
            QMessageBox::critical(this, "File Load Error", "Error loading game GPD, '" + gpdName + "'.\n\n" + QString::fromStdString(error));
            this->close();
            return;
        }
        GameEntry g = { gpd, &dashGPD->gamesPlayed.at(i), false, tempPath, gpdName.toStdString() };
        games.push_back(g);

        // if there are avatar awards then add it to the vector
        if (dashGPD->gamesPlayed.at(i).avatarAwardCount != 0)
        {
            AvatarAwardGameEntry a = { gpd, &dashGPD->gamesPlayed.at(i), NULL, false, string(""), string("") };
            aaGames.push_back(a);
        }

        // create the tree widget item
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdWString(gpd->gameName.ws));

        // set the thumbnail
        QByteArray imageBuff((char*)gpd->thumbnail.image, (size_t)gpd->thumbnail.length);
        item->setIcon(0, QIcon(QPixmap::fromImage(QImage::fromData(imageBuff))));

        // add the item to the list
        ui->gamesList->insertTopLevelItem(ui->gamesList->topLevelItemCount(), item);

        // add it to the avatar award game list if needed
        if (dashGPD->gamesPlayed.at(i).avatarAwardCount != 0)
            ui->aaGamelist->insertTopLevelItem(ui->aaGamelist->topLevelItemCount(), new QTreeWidgetItem(*item));
    }

    /////////////////////////
    // LOAD AVATAR AWARDS
    /////////////////////////

    // make sure the PEC file exists
    bool pecExists = profile->FileExists("PEC");
    if (!pecExists && aaGames.size() != 0)
    {
        QMessageBox::critical(this, "File Not Found", "Games have been found with avatar awards, but no PEC file exists.\n");
        ui->tabAvatarAwards->setEnabled(false);
        this->close();
        return;
    }
    else if (!pecExists)
    {
        ui->tabAvatarAwards->setEnabled(false);
        return;
    }

    // extract the PEC file
    pecTempPath = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
    profile->ExtractFile("PEC", pecTempPath);
    tempFiles.push_back(pecTempPath);

    // parse the PEC STFS package
    try
    {
        PEC = new StfsPackage(pecTempPath, true);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "File Load Error", "Error loading PEC file.\n\n" + QString::fromStdString(error));
        this->close();
        return;
    }

    // extract and parse all of the GPDs in the PEC
    for (DWORD i = 0; i < aaGames.size(); i++)
    {
        // get the name of the GPD in the PEC
        QString gpdName = QString::number(aaGames.at(i).titleEntry->titleID, 16).toUpper() + ".gpd";

        // make sure the GPD exists
        if (!PEC->FileExists(gpdName.toStdString()))
        {
            QMessageBox::critical(this, "File Not Error", "Avatar Award GPD '" + gpdName + "' was not found in the PEC.\n");
            this->close();
            return;
        }

        // extract the avatar award GPD
        string tempGPDName = (QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "")).toStdString();
        tempFiles.push_back(tempGPDName);
        PEC->ExtractFile(gpdName.toStdString(), tempGPDName);

        // parse the avatar award gpd
        AvatarAwardGPD *gpd;
        try
        {
            gpd = new AvatarAwardGPD(tempGPDName);
        }
        catch (string error)
        {
            QMessageBox::critical(this, "File Load Error", "Error loading the Avatar Award GPD '" + gpdName + "'.\n\n" + QString::fromStdString(error));
            this->close();
            return;
        }
        aaGames.at(i).gpd = gpd;
        aaGames.at(i).tempFileName = tempGPDName;
        aaGames.at(i).gpdName = gpdName.toStdString();
    }
}

void ProfileEditor::addToQueue(SettingEntryType type, UINT64 id)
{
    SettingEntry setting;
    setting.type = type;
    setting.entry.id = id;
    entriesToAdd.push_back(setting);
}

ProfileEditor::~ProfileEditor()
{
    // free all the game gpd memory
    for (DWORD i = 0; i < games.size(); i++)
        delete games.at(i).gpd;

    // free all of the avatar award gpds
    for (DWORD i = 0; i < aaGames.size(); i++)
        delete aaGames.at(i).gpd;

    // TODO: put a save button somewhere
    saveAll();

    delete dashGPD;

    if (dispose)
        delete profile;

    // delete all of the temp files
    for (DWORD i = 0; i < tempFiles.size(); i++)
        QFile::remove(QString::fromStdString(tempFiles.at(i)));
    delete ui;
}

void ProfileEditor::on_gamesList_itemSelectionChanged()
{
    if (ui->gamesList->currentIndex().row() < 0)
        return;

    GameGPD *curGPD = games.at(ui->gamesList->currentIndex().row()).gpd;
    ui->achievementsList->clear();

    // load all the achievements
    for (DWORD i = 0; i < curGPD->achievements.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdWString(curGPD->achievements.at(i).name).replace("\n", ""));
        item->setText(1, QString::fromStdWString((curGPD->achievements.at(i).flags & Unlocked) ? curGPD->achievements.at(i).unlockedDescription : curGPD->achievements.at(i).lockedDescription).replace("\n", ""));
        item->setText(2, QString::fromStdString(XDBFHelpers::GetAchievementState(&curGPD->achievements.at(i))));
        item->setText(3, QString::number(curGPD->achievements.at(i).gamerscore));

        ui->achievementsList->insertTopLevelItem(ui->achievementsList->topLevelItemCount(), item);
    }

    TitleEntry *title = games.at(ui->gamesList->currentIndex().row()).titleEntry;

    ui->lblGameName->setText("Name: " + QString::fromStdWString(title->gameName));
    ui->lblGameTitleID->setText("TitleID: " + QString::number(title->titleID, 16).toUpper());
    ui->lblGameLastPlayed->setText("Last Played: " + QDateTime::fromTime_t(title->lastPlayed).toString());
    ui->lblGameAchvs->setText("Achievements: " + QString::number(title->achievementsUnlocked) + " out of " + QString::number(title->achievementCount) + " unlocked");
    ui->lblGameGamerscore->setText("Gamerscore: " + QString::number(title->gamerscoreUnlocked) + " out of " + QString::number(title->totalGamerscore) + " unlocked");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedBoxArt(QNetworkReply*)));
    string tmp = DashboardGPD::GetSmallBoxArtURL(title);
    manager->get(QNetworkRequest(QUrl(QString::fromStdString(tmp))));
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
    if (ui->achievementsList->currentIndex().row() < 0 || ui->gamesList->currentIndex().row() < 0 || ui->achievementsList->currentIndex().row() >= games.at(ui->gamesList->currentIndex().row()).gpd->achievements.size())
        return;

    AchievementEntry entry = games.at(ui->gamesList->currentIndex().row()).gpd->achievements.at(ui->achievementsList->currentIndex().row());
    ui->lblAchName->setText("Name: " + QString::fromStdWString(entry.name));
    ui->lblAchLockDesc->setText("Locked Description: " + QString::fromStdWString(entry.lockedDescription));
    ui->lblAchUnlDesc->setText("Unlocked Description: " + QString::fromStdWString(entry.unlockedDescription));
    ui->lblAchType->setText("Type: " + QString::fromStdString(GameGPD::GetAchievementType(&entry)));
    ui->lblAchGamescore->setText("Gamerscore: " + QString::number(entry.gamerscore));
    ui->lblAchSecret->setText(QString("Secret: ") + ((entry.flags & Secret) ? "Yes" : "No"));

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

    ui->dteAchTimestamp->setDateTime(QDateTime::fromTime_t(entry.unlockTime));

    // set the thumbnail
    ImageEntry img;
    if (games.at(ui->gamesList->currentIndex().row()).gpd->GetAchievementThumbnail(&entry, &img))
    {
        QByteArray imageBuff((char*)img.image, (size_t)img.length);
        ui->imgAch->setPixmap(QPixmap::fromImage(QImage::fromData(imageBuff)));
    }
    else
        ui->imgAch->setPixmap(QPixmap::fromImage(QImage(":/Images/HiddenAchievement.png")));
}

void ProfileEditor::on_aaGamelist_itemSelectionChanged()
{
    if (ui->aaGamelist->currentIndex().row() < 0)
        return;

    ui->avatarAwardsList->clear();
    AvatarAwardGPD *gpd = aaGames.at(ui->aaGamelist->currentIndex().row()).gpd;

    // load all the avatar awards for the game
    for (DWORD i = 0; i < gpd->avatarAwards.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, QString::fromStdWString(gpd->avatarAwards.at(i).name));
        item->setText(1, QString::fromStdWString((gpd->avatarAwards.at(i).flags & Unlocked) ? gpd->avatarAwards.at(i).unlockedDescription : gpd->avatarAwards.at(i).lockedDescription));
        item->setText(2, QString::fromStdString(XDBFHelpers::AssetGenderToString(gpd->GetAssetGender(&gpd->avatarAwards.at(i)))));

        if (gpd->avatarAwards.at(i).flags & UnlockedOnline)
            item->setText(3, "Unlocked Online");
        else if (gpd->avatarAwards.at(i).flags & Unlocked)
            item->setText(3, "Unlocked Offline");
        else
            item->setText(3, "Locked");

        ui->avatarAwardsList->insertTopLevelItem(ui->avatarAwardsList->topLevelItemCount(), item);
    }

    TitleEntry *title = aaGames.at(ui->aaGamelist->currentIndex().row()).titleEntry;
    ui->lblAwGameName->setText("Name: " + QString::fromStdWString(title->gameName));
    ui->lblAwGameTitleID->setText("TitleID: " + QString::number(title->titleID, 16).toUpper());
    ui->lblAwGameLastPlayed->setText("Last Played: " + QDateTime::fromTime_t(title->lastPlayed).toString());
    ui->lblAwGameAwards->setText("Awards: " + QString::number(title->avatarAwardsEarned) + " out of " + QString::number(title->avatarAwardCount) + " unlocked");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedAwBoxArt(QNetworkReply*)));
    string tmp = DashboardGPD::GetSmallBoxArtURL(title);
    manager->get(QNetworkRequest(QUrl(QString::fromStdString(tmp))));
}

void ProfileEditor::replyFinishedAwBoxArt(QNetworkReply *aReply)
{
    QByteArray boxArt = aReply->readAll();
    if(boxArt.size() != 0 && !boxArt.contains("File not found."))
        ui->imgAwBoxArt->setPixmap(QPixmap::fromImage(QImage::fromData(boxArt)));
    else
        ui->imgAwBoxArt->setText("<i>Unable to download image.</i>");
}

void ProfileEditor::on_avatarAwardsList_itemSelectionChanged()
{
    if (ui->aaGamelist->currentIndex().row() < 0 || ui->avatarAwardsList->currentIndex().row() < 0 || ui->avatarAwardsList->currentIndex().row() >= aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->avatarAwards.size())
        return;

    // get the current award
    struct AvatarAward *award = &aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row());

    // update the ui
    ui->lblAwName->setText("Name: " + QString::fromStdWString(award->name));
    ui->lblAwLockDesc->setText("Locked Description: " + QString::fromStdWString(award->lockedDescription));
    ui->lblAwUnlDesc->setText("Unlocked Description: " + QString::fromStdWString(award->unlockedDescription));

    try
    {
         ui->lblAwType->setText("Type: " + QString::fromStdString(XDBFHelpers::AssetSubcategoryToString(award->subcategory)));
    }
    catch (...)
    {
        ui->lblAwType->setText("Type: <i>Unknown</i>");
    }

    ui->lblAwGender->setText("Gender: " + QString::fromStdString(XDBFHelpers::AssetGenderToString(aaGames.at(ui->aaGamelist->currentIndex().row()).gpd->GetAssetGender(award))));
    ui->lblAwSecret->setText(QString("Secret: ") + ((award->flags & 0xFFFF) ? "No" : "Yes"));

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

    ui->dteAwTimestamp->setDateTime(QDateTime::fromTime_t(award->unlockTime));

    // download the thumbnail
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedAwImg(QNetworkReply*)));
    string tmp = AvatarAwardGPD::GetLargeAwardImageURL(award);
    manager->get(QNetworkRequest(QUrl(QString::fromStdString(tmp))));
}

void ProfileEditor::replyFinishedAwImg(QNetworkReply *aReply)
{
    QByteArray img = aReply->readAll();
    if(img.size() != 0 && !img.contains("File not found."))
        ui->imgAw->setPixmap(QPixmap::fromImage(QImage::fromData(img)));
    else
        ui->imgAw->setText("<i>Unable to download image.</i>");
}

void ProfileEditor::on_btnUnlockAllAchvs_clicked()
{
    int index = ui->gamesList->currentIndex().row();
    if (index < 0)
        return;

    QMessageBox::StandardButton btn = QMessageBox::question(this, "Are you sure?", "Are you sure that you want to unlock all of the achievements for " +
                          QString::fromStdWString(games.at(index).titleEntry->gameName) + " offline?", QMessageBox::Yes, QMessageBox::No);

    if (btn != QMessageBox::Yes)
        return;

    games.at(index).gpd->UnlockAllAchievementsOffline();
    games.at(index).updated = true;

    // update the ui
    for (DWORD i = 0; i < ui->achievementsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->achievementsList->topLevelItem(i);
        if (item->text(2) != "Unlocked Online")
            item->setText(2, "Unlocked Offline");
    }

    games.at(index).titleEntry->achievementsUnlocked = games.at(index).titleEntry->achievementCount;
    games.at(index).titleEntry->gamerscoreUnlocked = games.at(index).titleEntry->totalGamerscore;
    games.at(index).titleEntry->flags |= (SyncAchievement | DownloadAchievementImage);

    ui->lblGameAchvs->setText("Achievements: " + QString::number(games.at(index).titleEntry->achievementsUnlocked) + " out of " + QString::number(games.at(index).titleEntry->achievementCount) + " unlocked");
    ui->lblGameGamerscore->setText("Gamerscore: " + QString::number(games.at(index).titleEntry->gamerscoreUnlocked) + " out of " + QString::number(games.at(index).titleEntry->totalGamerscore) + " unlocked");
}

void ProfileEditor::on_btnExtractGPD_clicked()
{
    DWORD index = ui->gamesList->currentIndex().row();
    if (index < 0)
        return;

    QString gpdName = QString::number(games.at(index).titleEntry->titleID, 16).toUpper() + ".gpd";
    QString filePath = QFileDialog::getSaveFileName(this, "Choose a place to save the GPD", QtHelpers::DesktopLocation() + "/" + gpdName, "GPD File (*.gpd *.fit);;All Files (*.*)");

    if (filePath == "")
        return;

    try
    {
        profile->ExtractFile(gpdName.toStdString(), filePath.toStdString());
        QMessageBox::information(this, "Success", "Successfully extracted " + gpdName + " from your profile.\n");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "Failed to extract " + gpdName + ".\n\n" + QString::fromStdString(error));
    }
}

void ProfileEditor::on_cmbxAchState_currentIndexChanged(const QString &arg1)
{
    // make sure the indices are valid
    if (ui->gamesList->currentIndex().row() < 0 || ui->achievementsList->currentIndex().row() < 0)
        return;

    // update the list text
    ui->achievementsList->topLevelItem(ui->achievementsList->currentIndex().row())->setText(2, arg1);

    GameGPD *gpd = games.at(ui->gamesList->currentIndex().row()).gpd;

    // update the achievement entry
    if (arg1 == "Locked")
    {
        // update the title entry
        updateAchievement(games.at(ui->gamesList->currentIndex().row()).titleEntry, &gpd->achievements.at(ui->achievementsList->currentIndex().row()), StateLocked, gpd);

        ui->dteAchTimestamp->setEnabled(false);
    }
    else if (arg1 == "Unlocked Offline")
    {
        // update the title entry
        updateAchievement(games.at(ui->gamesList->currentIndex().row()).titleEntry, &gpd->achievements.at(ui->achievementsList->currentIndex().row()), StateUnlockedOffline, gpd);

        ui->dteAchTimestamp->setEnabled(false);
    }
    else
    {
        // update the title entry
        updateAchievement(games.at(ui->gamesList->currentIndex().row()).titleEntry, &gpd->achievements.at(ui->achievementsList->currentIndex().row()), StateUnlockedOnline, gpd);

        ui->dteAchTimestamp->setEnabled(true);
    }

    games.at(ui->gamesList->currentIndex().row()).updated = true;
}

void ProfileEditor::on_btnExtractGPD_2_clicked()
{
    DWORD index = ui->aaGamelist->currentIndex().row();
    if (index < 0)
        return;

    QString gpdName = QString::number(aaGames.at(index).titleEntry->titleID, 16).toUpper() + ".gpd";
    QString filePath = QFileDialog::getSaveFileName(this, "Choose a place to save the GPD", QtHelpers::DesktopLocation() + "/" + gpdName, "GPD File (*.gpd *.fit);;All Files (*.*)");

    if (filePath == "")
        return;

    try
    {
        PEC->ExtractFile(gpdName.toStdString(), filePath.toStdString());
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

    QMessageBox::StandardButton btn = QMessageBox::question(this, "Are you sure?", "Are you sure that you want to unlock all of the avatar awards for " +
                          QString::fromStdWString(aaGames.at(index).titleEntry->gameName) + " offline?", QMessageBox::Yes, QMessageBox::No);

    if (btn != QMessageBox::Yes)
        return;

    aaGames.at(index).gpd->UnlockAllAwards();
    aaGames.at(index).updated = true;

    // update the ui
    for (DWORD i = 0; i < ui->avatarAwardsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->avatarAwardsList->topLevelItem(i);
        if (item->text(3) != "Unlocked Online")
            item->setText(3, "Unlocked Offline");
    }

    // update the title entry
    aaGames.at(index).titleEntry->avatarAwardsEarned = aaGames.at(index).titleEntry->avatarAwardCount;
    aaGames.at(index).titleEntry->maleAvatarAwardsEarned = aaGames.at(index).titleEntry->maleAvatarAwardCount;
    aaGames.at(index).titleEntry->femaleAvatarAwardsEarned = aaGames.at(index).titleEntry->femaleAvatarAwardCount;
    aaGames.at(index).titleEntry->flags |= (DownloadAvatarAward | SyncAvatarAward);

    ui->lblAwGameAwards->setText("Awards: " + QString::number(aaGames.at(index).titleEntry->avatarAwardsEarned) + " out of " + QString::number(aaGames.at(index).titleEntry->avatarAwardCount) + " unlocked");
}

void ProfileEditor::on_cmbxAwState_currentIndexChanged(const QString &arg1)
{
    // make sure the indices are valid
    if (ui->aaGamelist->currentIndex().row() < 0 || ui->avatarAwardsList->currentIndex().row() < 0)
        return;

    // update the list text
    ui->avatarAwardsList->topLevelItem(ui->avatarAwardsList->currentIndex().row())->setText(3, arg1);

    AvatarAwardGPD *gpd = aaGames.at(ui->aaGamelist->currentIndex().row()).gpd;

    // update the award entry
    if (arg1 == "Locked")
    {
        // update the title entry
        updateAvatarAward(aaGames.at(ui->aaGamelist->currentIndex().row()).titleEntry, gpd, &gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row()), StateLocked);
        ui->dteAchTimestamp->setEnabled(false);
    }
    else if (arg1 == "Unlocked Offline")
    {
        // update the title entry
        updateAvatarAward(aaGames.at(ui->aaGamelist->currentIndex().row()).titleEntry, gpd, &gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row()), StateUnlockedOffline);

        ui->dteAchTimestamp->setEnabled(false);
    }
    else
    {
        // update the title entry
        updateAvatarAward(aaGames.at(ui->aaGamelist->currentIndex().row()).titleEntry, gpd, &gpd->avatarAwards.at(ui->avatarAwardsList->currentIndex().row()), StateUnlockedOnline);

        ui->dteAchTimestamp->setEnabled(true);
    }

    aaGames.at(ui->aaGamelist->currentIndex().row()).updated = true;
}

void ProfileEditor::updateAvatarAward(TitleEntry *entry, AvatarAwardGPD *gpd, struct AvatarAward *award, State toSet)
{
    AssetGender g = AvatarAwardGPD::GetAssetGender(award);
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
        award->flags |= Unlocked;
    }
    else if (toSet == StateUnlockedOnline)
        award->flags |= (Unlocked | UnlockedOnline);

    // write the entry back to the gpd
    gpd->WriteAvatarAward(award);

    // update the dash gpd
    dashGPD->WriteTitleEntry(entry);

    entry->flags |= (DownloadAvatarAward | SyncAvatarAward);
}

void ProfileEditor::updateAchievement(TitleEntry *entry, AchievementEntry *chiev, State toSet, GameGPD *gpd)
{
    try
    {
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
            entry->achievementsUnlocked++;
            if (toSet == StateUnlockedOnline)
                entry->achievementsUnlockedOnline++;

            entry->gamerscoreUnlocked += chiev->gamerscore;
            dashGPD->gamerscoreUnlocked.int32 += chiev->gamerscore;
            dashGPD->achievementsUnlocked.int32++;

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
            dashGPD->gamerscoreUnlocked.int32 -= chiev->gamerscore;
            dashGPD->achievementsUnlocked.int32--;

            settingAchievements.int32--;
            settingGamerscore.int32 -= chiev->gamerscore;

            gpd->WriteSettingEntry(settingAchievements);
            gpd->WriteSettingEntry(settingGamerscore);

            chiev->flags &= 0xFFFCFFFF;
        }

        if (toSet == StateUnlockedOffline)
        {
            chiev->flags &= 0xFFFCFFFF;
            chiev->flags |= Unlocked;
        }
        else if (toSet == StateUnlockedOnline)
            chiev->flags |= (Unlocked | UnlockedOnline);

        chiev->flags |= (SyncAchievement | DownloadAchievementImage);

        gpd->WriteAchievementEntry(chiev);

        dashGPD->WriteTitleEntry(entry);
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while updating the achievement.\n\n" + QString::fromStdString(error));
    }
}

void ProfileEditor::saveAll()
{
    // create all the entries that need to be created
   /* for (DWORD i = 0; i < entriesToAdd.size(); i++)
        dashGPD->CreateSettingEntry(&entriesToAdd.at(i), entriesToAdd.at(i).entry.id); */

    // save the avatar awards
    if (PEC != NULL)
    {
        // put all the avatar award gpds back in the PEC
        for (DWORD i = 0; i < aaGames.size(); i++)
            if (aaGames.at(i).updated)
                PEC->ReplaceFile(aaGames.at(i).tempFileName, aaGames.at(i).gpdName);

        PEC->Rehash();

        delete PEC;

        profile->ReplaceFile(pecTempPath, "PEC");
    }

    // save the achievements
    for (DWORD i = 0; i < games.size(); i++)
        if (games.at(i).updated)
            profile->ReplaceFile(games.at(i).tempFileName, games.at(i).gpdName);


    // put the dash gpd back in the profile
    profile->ReplaceFile(dashGPDTempPath, "FFFE07D1.gpd");

    // fix the package
    profile->Rehash();
    // TODO: resign yo
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

void ProfileEditor::getAvatarColor(QPushButton *sender)
{
    QColor color = QColorDialog::getColor(sender->palette().background().color(), this);
    sender->setStyleSheet("* { background-color: rgb(" + QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue()) + ") }");
}

void ProfileEditor::on_clrEyeShadow_clicked()
{
    getAvatarColor(ui->clrEyeShadow);
}

void ProfileEditor::on_clrSkin_clicked()
{
    getAvatarColor(ui->clrSkin);
}

void ProfileEditor::on_clrHair_clicked()
{
    getAvatarColor(ui->clrHair);
}

void ProfileEditor::on_clrLips_clicked()
{
    getAvatarColor(ui->clrLips);
}

void ProfileEditor::on_clrEyes_clicked()
{
    getAvatarColor(ui->clrEyes);
}

void ProfileEditor::on_clrEyebrows_clicked()
{
    getAvatarColor(ui->clrEyebrows);
}

void ProfileEditor::on_clrFacialHair_clicked()
{
    getAvatarColor(ui->clrFacialHair);
}

void ProfileEditor::on_clrFacePaint_clicked()
{
    getAvatarColor(ui->clrFacePaint);
}

void ProfileEditor::on_clrfacePaint2_clicked()
{
    getAvatarColor(ui->clrfacePaint2);
}
