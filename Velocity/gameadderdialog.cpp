#include "gameadderdialog.h"
#include "ui_gameadderdialog.h"

Q_DECLARE_METATYPE(TitleEntry)

GameAdderDialog::GameAdderDialog(StfsPackage *package, QWidget *parent, bool dispose) : QDialog(parent), ui(new Ui::GameAdderDialog), package(package), dispose(dispose)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

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
        dashGPDTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        package->ExtractFile("FFFE07D1.gpd", dashGPDTempPath);
        dashGPD = new DashboardGPD(dashGPDTempPath);
    }
    catch(const QString &error)
    {
        QMessageBox::critical(this, "File Error", "The dashboard GPD could not be extracted/opened.\n\n" + error);
        this->close();
        return;
    }

    pecTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
    mainDir = "http://velocity.expetelek.com/gameadder/";

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gameReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(mainDir + "listing.php")));

    // setup the context menus
    ui->treeWidgetAllGames->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidgetQueue->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeWidgetAllGames, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu_AllGames(QPoint)));
    connect(ui->treeWidgetQueue, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveContextMenu_QueuedGames(QPoint)));
}

GameAdderDialog::~GameAdderDialog()
{
    delete ui;
}

void GameAdderDialog::gameReplyFinished(QNetworkReply *aReply)
{
    QString jsonStr(aReply->readAll());

    QVector<TitleEntry> gamesPlayed = dashGPD->gamesPlayed;

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
        for (int i = 0; i < gamesPlayed.size(); i++)
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

        TitleEntry entry;
        entry.gameName = gameName.toStdWString();
        entry.titleID = titleId;
        entry.achievementCount = achievementCount.toULong();
        entry.totalGamerscore = totalGamerscore.toULong();
        entry.avatarAwardCount = (BYTE)totalAwardCount.toInt();
        entry.maleAvatarAwardCount = (BYTE)maleAwardCount.toInt();
        entry.femaleAvatarAwardCount = (BYTE)femaleAwardCount.toInt();

        entry.achievementsUnlocked = 0;
        entry.achievementsUnlockedOnline = 0;
        entry.avatarAwardsEarned = 0;
        entry.femaleAvatarAwardsEarned = 0;
        entry.maleAvatarAwardsEarned = 0;
        entry.gamerscoreUnlocked = 0;
        entry.lastPlayed = QDateTime::currentDateTime().toTime_t();

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
}

void GameAdderDialog::thumbnailReplyFinished(QNetworkReply *aReply)
{
    QByteArray thumbnail = aReply->readAll();
    if(thumbnail.size() != 0 && !thumbnail.contains("File not found."))
        ui->imgThumbnail->setPixmap(QPixmap::fromImage(QImage::fromData(thumbnail)));
    else
        ui->imgThumbnail->setText("<i>Unable to download image.</i>");
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
    }
}

void GameAdderDialog::finishedDownloadingGPD(QString gamePath, QString awardPath, TitleEntry entry, bool error)
{
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
            // inject the game gpd
            m.lock();
            package->InjectFile(gamePath, gpdName);
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
                        FileEntry pecEntry = package->GetFileEntry("PEC");
                        if (pecEntry.blocksForFile == 1)
                        {
                            flags |= StfsPackageCreate;
                            existed = true;
                        }
                        else
                        {
                            m.lock();
                            package->ExtractFile("PEC", pecTempPath);
                            m.unlock();
                            existed = true;
                        }
                    }

                    pecPackage = new StfsPackage(pecTempPath, flags);
                }

                // inject the gpd and delete it
                m.lock();
                pecPackage->InjectFile(awardPath, gpdName);
                m.unlock();
                QFile::remove(awardPath);
            }

            // update the dash gpd
            m.lock();
            dashGPD->CreateTitleEntry(&entry);
            dashGPD->gamePlayedCount.int32++;
            m.unlock();
        }
        catch (const QString &error)
        {
            QMessageBox::critical(this, "Error Occured", "An error occured while injecting " + gpdName + ".\n\n" + error);
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
            dashGPD->WriteSettingEntry(dashGPD->gamePlayedCount);
            m.unlock();
        }
        catch (const QString &error)
        {
            QMessageBox::critical(this, "Error Writing Entry", "The entry was not written successfully.\n\n" + error);
            close();
        }
        dashGPD->Close();

        try
        {
            m.lock();
            package->ReplaceFile(dashGPDTempPath, "FFFE07D1.gpd");
            m.unlock();
            QFile::remove(dashGPDTempPath);
        }
        catch (const QString &error)
        {
            QMessageBox::critical(this, "Error Replacing GPD", "The dashboard GPD could not be replaced.\n\n" + error);
            close();
        }

        QString kvPath = QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType);
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
                    package->InjectFile(pecTempPath, "PEC");
                else
                    package->ReplaceFile(pecTempPath, "PEC");

                QFile::remove(pecTempPath);
                m.unlock();
            }
        }
        catch (const QString &error)
        {
            QMessageBox::critical(this, "PEC Error" , "The PEC file could not be replaced/injected.\n\n" + error);
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
            }
        }
        catch (const QString &error)
        {
            QMessageBox::critical(this, "Fixing Error" , "The package could not be rehashed/resigned.\n\n" + error);
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

    TitleEntry entry = current->data(0, Qt::UserRole).value<TitleEntry>();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(thumbnailReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://image.xboxlive.com/global/t." + QString::number(entry.titleID, 16) + "/icon/0/8000")));

    ui->lblGameName->setText("" + QString::fromStdWString(entry.gameName));
    ui->lblTitleID->setText("<span style=\"color:#4f4f4f;\">" + QString::number(entry.titleID, 16).toUpper() + "</span>");
    ui->lblGamerscore->setText("" + QString::number(entry.totalGamerscore));
    ui->lblAvatarAwards->setText("" + QString::number(entry.avatarAwardCount));
    ui->lblAchievements->setText(QString::number(entry.achievementCount));
}

void GameAdderDialog::on_pushButton_clicked()
{
    if (pecPackage != NULL)
    {
        pecPackage->Close();
        QFile::remove(pecTempPath);
    }

    dashGPD->Close();
    QFile::remove(dashGPDTempPath);
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
    if (dashGPD->gamePlayedCount.entry.type == 0)
    {
        dashGPD->gamePlayedCount.type = Int32;
        dashGPD->gamePlayedCount.int32 = 0;
        dashGPD->CreateSettingEntry(&dashGPD->gamePlayedCount, GamercardTitlesPlayed);
    }

    for (int i = 0; i < totalCount; i++)
    {
        TitleEntry entry = ui->treeWidgetQueue->topLevelItem(i)->data(0, Qt::UserRole).value<TitleEntry>();

        if (!package->FileExists(QString::number(entry.titleID, 16).toUpper() + ".gpd"))
        {
            GPDDownloader *downloader = new GPDDownloader(entry, entry.avatarAwardCount != 0, this);
            connect(downloader, SIGNAL(FinishedDownloading(QString, QString, TitleEntry, bool)), this, SLOT(finishedDownloadingGPD(QString, QString, TitleEntry, bool)));
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
