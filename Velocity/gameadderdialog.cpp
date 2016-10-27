#include "gameadderdialog.h"
#include "ui_gameadderdialog.h"

Q_DECLARE_METATYPE(TitleEntry)

GameAdderDialog::GameAdderDialog(StfsPackage *package, QWidget *parent, bool dispose, bool *ok) : QDialog(parent), ui(new Ui::GameAdderDialog), package(package), dispose(dispose)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    warned = false;

    QFont f = ui->lblGameName->font();
#ifdef __WIN32
    f.setPointSize(10);
#endif
    ui->lblGameName->setFont(f);

    allowInjection = false;

    pecPackage = NULL;

    ui->treeWidgetAllGames->header()->resizeSection(0, 182);
    ui->treeWidgetQueue->header()->resizeSection(0, 182);

    // make sure the dashboard gpd exists
    try
    {
        dashGpdTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        package->ExtractFile("FFFE07D1.gpd", dashGpdTempPath.toStdString());
        dashGpd = new DashboardGpd(dashGpdTempPath.toStdString());
    }
    catch(string error)
    {
        QMessageBox::critical(this, "File Error", "The dashboard Gpd could not be extracted/opened.\n\n" + QString::fromStdString(error));
        this->close();
        return;
    }

    pecTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

    manager = new QNetworkAccessManager(this);

    // check for a connection
    if (manager->networkAccessible() == QNetworkAccessManager::NotAccessible)
    {
        QMessageBox::warning(this, "Listing Error", "The listing could not be parsed. Try again later, as the servers may be down and make sure Velocity has access to the internet.");
        *ok = false;
        return;
    }

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gameReplyFinished(QNetworkReply*)));
    getParams = "";
    getListing();

    // setup the context menus
    ui->treeWidgetAllGames->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidgetQueue->setContextMenuPolicy(Qt::CustomContextMenu);

    imageManager = new QNetworkAccessManager(this);
    connect(imageManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(thumbnailReplyFinished(QNetworkReply*)));

    connect(ui->treeWidgetAllGames, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu_AllGames(QPoint)));
    connect(ui->treeWidgetQueue, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu_QueuedGames(QPoint)));

    *ok = true;
}

GameAdderDialog::~GameAdderDialog()
{
    delete ui;
}

void GameAdderDialog::gameReplyFinished(QNetworkReply *aReply)
{
    QString jsonStr(aReply->readAll());

    vector<TitleEntry> gamesPlayed = dashGpd->gamesPlayed;

    bool ok;
    QVariantMap result = QtJson::Json::parse(jsonStr, ok).toMap();

    if (!ok)
    {
        ui->tabWidget->setEnabled(false);
        QMessageBox::warning(this, "Listing Error", "The listing could not be parsed. Try again later, as the servers may be down and make sure Velocity has access to the internet.");
        return;
    }

    foreach (QVariant game, result["games"].toList())
    {
        QVariantMap gameMap = game.toMap();
        QString gameName = gameMap["nm"].toString();

        DWORD titleId = gameMap["tid"].toString().toULong(0, 16);
        bool alreadyExists = false;
        for (unsigned int i = 0; i < gamesPlayed.size(); i++)
            if (gamesPlayed.at(i).titleID == titleId)
            {
                gamesPlayed.erase(gamesPlayed.begin() + i);
                alreadyExists = true;
                break;
            }

        if (alreadyExists)
            continue;

        QString achievementCount = gameMap["achc"].toString();
        QString totalGamerscore = gameMap["ttlgs"].toString();
        QString totalAwardCount = gameMap["ttlac"].toString();
        QString maleAwardCount = gameMap["ttlmac"].toString();
        QString femaleAwardCount = gameMap["ttlfac"].toString();

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetAllGames);
        item->setText(0, gameName.trimmed());
        item->setText(1, totalGamerscore);
        item->setText(2, totalAwardCount);

        TitleEntry entry = { (EntryType)0 };
        entry.initialLength = 0;
        entry.titleID = titleId;
        entry.achievementCount = (DWORD)achievementCount.toULong();
        entry.achievementsUnlocked = 0;
        entry.totalGamerscore = (DWORD)totalGamerscore.toULong();
        entry.gamerscoreUnlocked = 0;
        entry.achievementsUnlockedOnline = 0;
        entry.avatarAwardsEarned = 0;
        entry.avatarAwardCount = (BYTE)totalAwardCount.toInt();
        entry.maleAvatarAwardsEarned = 0;
        entry.maleAvatarAwardCount = (BYTE)maleAwardCount.toInt();
        entry.femaleAvatarAwardsEarned = 0;
        entry.femaleAvatarAwardCount = (BYTE)femaleAwardCount.toInt();
        entry.flags = 0;
        entry.lastPlayed = QDateTime::currentDateTime().toTime_t();
        entry.gameName = gameName.toStdWString();

        item->setData(0, Qt::UserRole, QVariant::fromValue(entry));

        ui->treeWidgetAllGames->addTopLevelItem(item);
    }

    if (ui->treeWidgetAllGames->topLevelItemCount() > 0)
    {
        ui->treeWidgetAllGames->setCurrentItem(ui->treeWidgetAllGames->topLevelItem(0));
        ui->treeWidgetAllGames->topLevelItem(0)->setSelected(true);
    }

    // sort the items alphabetically
    ui->treeWidgetAllGames->sortByColumn(0, Qt::AscendingOrder);

    ui->tabWidget->setEnabled(true);
}

void GameAdderDialog::thumbnailReplyFinished(QNetworkReply *aReply)
{
    int count = ui->treeWidgetAllGames->selectedItems().count();
    if (count != 1)
        ui->imgThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
    else
    {
        QByteArray thumbnail = aReply->readAll();
        if(thumbnail.size() != 0 && !thumbnail.contains("File not found."))
            ui->imgThumbnail->setPixmap(QPixmap::fromImage(QImage::fromData(thumbnail)));
        else
            ui->imgThumbnail->setText("<i>Unable to download image.</i>");
    }
}

void GameAdderDialog::showRemoveContextMenu_AllGames(QPoint point)
{
    int amount = ui->treeWidgetAllGames->selectedItems().length();
    if (amount < 1)
        return;

    QPoint globalPos = ui->treeWidgetAllGames->mapToGlobal(point);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/add.png"), "Add to Queue");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    QList <QTreeWidgetItem*> items = ui->treeWidgetAllGames->selectedItems();
    if (items.count() < 1)
        return;

    if (selectedItem->text() == "Add to Queue")
    {
        for (int i = 0; i < items.count(); i++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetQueue);
            item->setText(0, items.at(i)->text(0));
            item->setText(1, items.at(i)->text(1));
            item->setText(2, items.at(i)->text(2));
            item->setData(0, Qt::UserRole, items.at(i)->data(0, Qt::UserRole));
            ui->treeWidgetQueue->addTopLevelItem(item);

            allowInjection = true;

            delete items.at(i);
        }
        ui->lblAchievements->setText("N/A");
        ui->lblAvatarAwards->setText("N/A");
        ui->lblGameName->setText("");
        ui->lblGamerscore->setText("N/A");
        ui->lblTitleID->setText("");
        ui->imgThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
    }
}

void GameAdderDialog::finishedDownloadingGpd(QString gamePath, QString awardPath, TitleEntry entry, bool error)
{
    GpdDownloader *downloader = (GpdDownloader*)sender();
    int index = downloader->index();
    delete downloader;

    ui->progressBar->setValue(((double)++downloadedCount / (double)totalDownloadCount) * 100);
    QMutex m;
    if (!error)
    {
        if (!allowInjection)
        {
            QFile::remove(gamePath);

            if (!awardPath.isEmpty())
                QFile::remove(awardPath);

            return;
        }

        QString gpdName = QString::number(entry.titleID, 16).toUpper() + ".gpd";
        try
        {
            qDebug() << "Started " << QString::fromStdWString(entry.gameName);
            // inject the game gpd
            m.lock();
            package->InjectFile(gamePath.toStdString(), gpdName.toStdString());
            m.unlock();
            QFile::remove(gamePath);

            if (!awardPath.isEmpty())
            {
                if  (pecPackage == NULL)
                {
                    DWORD flags = StfsPackagePEC;

                    if (!package->FileExists("PEC"))
                    {
                        flags |= StfsPackageCreate;
                        existed = false;
                    }
                    else
                    {
                        StfsFileEntry pecEntry = package->GetFileEntry("PEC");
                        if (pecEntry.blocksForFile < 3)
                        {
                            flags |= StfsPackageCreate;
                            existed = true;
                        }
                        else
                        {
                            m.lock();
                            package->ExtractFile("PEC", pecTempPath.toStdString());
                            m.unlock();
                            existed = true;
                        }
                    }

                    pecPackage = new StfsPackage(pecTempPath.toStdString(), flags);
                }

                // inject the gpd and delete it
                m.lock();
                pecPackage->InjectFile(awardPath.toStdString(), gpdName.toStdString());
                m.unlock();
                QFile::remove(awardPath);
            }

            // update the dash gpd
            m.lock();
            dashGpd->CreateTitleEntry(&entry);
            ui->treeWidgetQueue->topLevelItem(index)->setData(0, Qt::UserRole, QVariant::fromValue(entry));
            dashGpd->gamePlayedCount.int32++;
            m.unlock();

            qDebug() << "Ended " << QString::fromStdWString(entry.gameName);
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error Occured", "An error occured while injecting " + gpdName + ".\n\n" + QString::fromStdString(error));
        }
        catch(...)
        {
            QMessageBox::critical(this, "Error Occured", "An unknown error occured while injecting the game(s).");
        }
    }
    else
        notSuccessful.append(QString::fromStdWString(entry.gameName));

    if (downloadedCount == totalDownloadCount)
    {
        if (notSuccessful.size() > 0)
        {
            QString games = "";
            for (int i = 0; i < notSuccessful.size(); i++)
                games += notSuccessful.at(i) + "\n";

            QMessageBox::warning(this, "Warning", "Not all games could be added successfully! The following game(s) failed:\n" + games);
        }

        try
        {
            m.lock();
            dashGpd->WriteSettingEntry(dashGpd->gamePlayedCount);
            m.unlock();
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error Writing Entry", "The entry was not written successfully.\n\n" + QString::fromStdString(error));
            close();
        }

        // make sure that all of the games were added correctly
        bool problems = false;
        for (int i = 0; i < ui->treeWidgetQueue->topLevelItemCount(); i++)
        {
            TitleEntry entry = ui->treeWidgetQueue->topLevelItem(i)->data(0, Qt::UserRole).value<TitleEntry>();
            bool exists = true;
            if (!package->FileExists(QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd"))
            {
                exists = false;
                try { dashGpd->DeleteTitleEntry(&entry); }
                catch (std::string error) { qDebug() << "Problem " << QString::fromStdString(error); }
                catch(...) { qDebug() << "Problem unknown"; }

                if (pecPackage->FileExists(QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd"))
                    pecPackage->RemoveFile(QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd");
                problems = true;
                qDebug() << "Problem " << QString::fromStdWString(entry.gameName);

            }
            if (entry.avatarAwardCount != 0 && !pecPackage->FileExists(QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd"))
            {
                try { dashGpd->DeleteTitleEntry(&entry); }
                catch (std::string error) { qDebug() << "Problem " << QString::fromStdString(error); }
                catch(...) { qDebug() << "Problem unknown"; }

                if (exists)
                    package->RemoveFile(QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd");

                problems = true;
                qDebug() << "Problem " << QString::fromStdWString(entry.gameName);
            }
        }

        if (problems)
            QMessageBox::warning(this, "Error Adding Games", "Some of the games weren't added correctly.");

        try
        {
            dashGpd->Close();
            delete dashGpd;

            m.lock();
            package->ReplaceFile(dashGpdTempPath.toStdString(), "FFFE07D1.gpd");
            m.unlock();

            QFile::remove(dashGpdTempPath);
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error Replacing Gpd", "The dashboard Gpd could not be replaced.\n\n" + QString::fromStdString(error));
            close();
        }

        std::string kvPath = QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType);
        try
        {
            if (pecPackage != NULL)
            {   
                // make sure the profile IDs match
                memcpy(pecPackage->metaData->profileID, package->metaData->profileID, 8);

                // make sure this bool is true
                pecPackage->metaData->enabled = true;
                pecPackage->metaData->WriteMetaData();

                m.lock();
                pecPackage->Rehash();
                pecPackage->Resign(kvPath);

                pecPackage->Close();

                if (!existed)
                    package->InjectFile(pecTempPath.toStdString(), "PEC");
                else
                    package->ReplaceFile(pecTempPath.toStdString(), "PEC");

                QFile::remove(pecTempPath);
                m.unlock();
            }
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "PEC Error" , "The PEC file could not be replaced/injected.\n\n" + QString::fromStdString(error));
            close();
        }

        try
        {
            m.lock();
            package->Rehash();
            package->Resign(kvPath);
            m.unlock();

            if (dispose)
            {
                package->Close();
                delete package;

                if (pecPackage != NULL)
                    delete pecPackage;
            }
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Fixing Error" , "The package could not be rehashed/resigned.\n\n" + QString::fromStdString(error));
        }

        close();
    }
}

void GameAdderDialog::showRemoveContextMenu_QueuedGames(QPoint point)
{
    int amount = ui->treeWidgetQueue->selectedItems().length();
    if (amount < 1)
        return;

    QPoint globalPos = ui->treeWidgetQueue->mapToGlobal(point);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/delete.png"), "Remove from Queue");

    QAction *selectedItem = contextMenu.exec(globalPos);
    if(selectedItem == NULL)
        return;

    QList <QTreeWidgetItem*> items = ui->treeWidgetQueue->selectedItems();
    if (items.count() < 1)
        return;

    if (selectedItem->text() == "Remove from Queue")
    {
        for (int i = 0; i < items.count(); i++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetAllGames);
            item->setText(0, items.at(i)->text(0));
            item->setText(1, items.at(i)->text(1));
            item->setText(2, items.at(i)->text(2));
            item->setData(0, Qt::UserRole, items.at(i)->data(0, Qt::UserRole));
            ui->treeWidgetAllGames->addTopLevelItem(item);

            delete items.at(i);
        }
    }
}

void GameAdderDialog::on_treeWidgetAllGames_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem * /* previous */)
{
    if (current == NULL)
    {
        ui->lblGameName->setText("<i>N/A</i>");
        ui->lblTitleID->setText("<span style=\"color:#4f4f4f;\"><i>N/A</i></span>");
        ui->imgThumbnail->setPixmap(QPixmap());
        return;
    }

    if (ui->treeWidgetAllGames->selectedItems().count() == 1)
    {
        TitleEntry entry = current->data(0, Qt::UserRole).value<TitleEntry>();

        imageManager->get(QNetworkRequest(QUrl("http://image.xboxlive.com/global/t." + QString::number(entry.titleID, 16) + "/icon/0/8000")));

        ui->lblGameName->setText("" + QString::fromStdWString(entry.gameName));
        ui->lblTitleID->setText("<span style=\"color:#4f4f4f;\">" + QString::number(entry.titleID, 16).toUpper() + "</span>");
        ui->lblGamerscore->setText("" + QString::number(entry.totalGamerscore));
        ui->lblAvatarAwards->setText("" + QString::number(entry.avatarAwardCount));
        ui->lblAchievements->setText(QString::number(entry.achievementCount));
    }
    else
    {
        ui->lblAchievements->setText("N/A");
        ui->lblAvatarAwards->setText("N/A");
        ui->lblGameName->setText("");
        ui->lblGamerscore->setText("N/A");
        ui->lblTitleID->setText("");
        ui->imgThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
    }
}

void GameAdderDialog::on_pushButton_clicked()
{
    if (pecPackage != NULL)
    {
        pecPackage->Close();
        delete pecPackage;
        QFile::remove(pecTempPath);
    }

    dashGpd->Close();
    QFile::remove(dashGpdTempPath);
    this->close();
}

void GameAdderDialog::on_pushButton_2_clicked()
{
    int totalCount = ui->treeWidgetQueue->topLevelItemCount();
    if (totalCount < 1)
    {
        QMessageBox::warning(this, "No Games", "You have selected no games to add.");
        return;
    }

    totalDownloadCount = totalCount;
    downloadedCount = 0;

    // create the setting entry if it doesn't exist
    if (dashGpd->gamePlayedCount.entry.type == 0)
    {
        dashGpd->gamePlayedCount.type = Int32;
        dashGpd->gamePlayedCount.int32 = 0;
        dashGpd->CreateSettingEntry(&dashGpd->gamePlayedCount, GamercardTitlesPlayed);
    }

    for (int i = 0; i < totalCount; i++)
    {
        TitleEntry entry = ui->treeWidgetQueue->topLevelItem(i)->data(0, Qt::UserRole).value<TitleEntry>();

        if (!package->FileExists(QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd"))
        {
            GpdDownloader *downloader = new GpdDownloader(entry, i, entry.avatarAwardCount != 0, this);
            connect(downloader, SIGNAL(FinishedDownloading(QString, QString, TitleEntry, bool)), this, SLOT(finishedDownloadingGpd(QString, QString, TitleEntry, bool)));
            downloader->BeginDownload();
        }
        else
            downloadedCount++;
    }

    if (downloadedCount == totalDownloadCount)
    {
        if (dispose)
        {
            package->Close();
            delete package;
        }

        close();
    }
}

void GameAdderDialog::showAllItems()
{
    for (int i = 0; i < ui->treeWidgetAllGames->topLevelItemCount(); i++)
        ui->treeWidgetAllGames->topLevelItem(i)->setHidden(false);
}

void GameAdderDialog::getListing()
{
    ui->treeWidgetAllGames->clear();
    ui->tabWidget->setEnabled(false);
    manager->get(QNetworkRequest(QUrl("http://velocity.expetelek.com/gameadder/listing.php" + getParams)));
}

void GameAdderDialog::on_btnShowAll_clicked()
{
    ui->txtSearch->setText("");
    ui->btnShowAll_2->setChecked(false);
    showAllItems();
}

void GameAdderDialog::on_txtSearch_textChanged(const QString & /* arg1 */)
{
    QList<QTreeWidgetItem*> itemsMatched = ui->treeWidgetAllGames->findItems(ui->txtSearch->text(), Qt::MatchContains | Qt::MatchRecursive);

    // hide all the items
    for (int i = 0; i < ui->treeWidgetAllGames->topLevelItemCount(); i++)
        ui->treeWidgetAllGames->setItemHidden(ui->treeWidgetAllGames->topLevelItem(i), true);

    if (itemsMatched.count() == 0)
    {
        ui->txtSearch->setStyleSheet("color: rgb(255, 1, 1);");
        showAllItems();
        return;
    }

    ui->txtSearch->setStyleSheet("");
    // add all the matched ones to the list
    for (int i = 0; i < itemsMatched.count(); i++)
    {
        // show all the item's parents
        QTreeWidgetItem *parent = itemsMatched.at(i)->parent();
        while (parent != NULL)
        {
            ui->treeWidgetAllGames->setItemHidden(parent, false);
            parent->setExpanded(true);
            parent = parent->parent();
        }

        // show the item itself
        ui->treeWidgetAllGames->setItemHidden(itemsMatched.at(i), false);
    }
}

void GameAdderDialog::on_btnShowAll_2_clicked(bool checked)
{
    for (int i = 0; i < ui->treeWidgetAllGames->topLevelItemCount(); i++)
    {
        TitleEntry entry = ui->treeWidgetAllGames->topLevelItem(i)->data(0, Qt::UserRole).value<TitleEntry>();
        if (entry.avatarAwardCount == 0)
            ui->treeWidgetAllGames->topLevelItem(i)->setHidden(checked);
    }
}

void GameAdderDialog::on_pushButton_3_clicked()
{
    bool developerMode = ui->pushButton_3->isChecked();
    if (developerMode)
    {
        if (!warned)
        {
            warned = true;
            QMessageBox::StandardButton selection = (QMessageBox::StandardButton)QMessageBox::warning(this, "Developer Mode?", "Developer mode will display games that have not yet been approved by the Velocity developers. With this said, these games could potentially corrupt your profile. Would you like to continue?", QMessageBox::Yes, QMessageBox::No);
            if (selection == QMessageBox::No)
            {
                ui->pushButton_3->setChecked(false);
                getParams = "";
                return;
            }
            else
                getParams = "?dev=1";
        }
        else
            getParams = "?dev=1";
    }
    else
        getParams = "";

    getListing();
}
