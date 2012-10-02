#include "gameadderdialog.h"
#include "ui_gameadderdialog.h"

Q_DECLARE_METATYPE(TitleEntry)

GameAdderDialog::GameAdderDialog(StfsPackage *package, QWidget *parent) : QDialog(parent), ui(new Ui::GameAdderDialog), package(package)
{
    ui->setupUi(this);

    allowInjection = false;

    pecPackage = NULL;

    ui->treeWidgetAllGames->header()->resizeSection(0, 182);
    ui->treeWidgetQueue->header()->resizeSection(0, 182);

    // make sure the dashboard gpd exists

    try
    {
        dashGPDTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
        package->ExtractFile("FFFE07D1.gpd", dashGPDTempPath.toStdString());
        dashGPD = new DashboardGPD(dashGPDTempPath.toStdString());
    }
    catch(string error)
    {
        QMessageBox::critical(this, "File Error", "The dashboard GPD could not be extracted/opened.\n\n" + QString::fromStdString(error));
        this->close();
        return;
    }

    pecTempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
    mainDir = "http://127.0.0.1/";

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gameReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(mainDir + "games.xml")));

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
    QByteArray gameListXML = aReply->readAll();
    QDomDocument doc;
    doc.setContent(gameListXML);

    QDomNodeList elem = doc.firstChild().childNodes();
    for (int i = 0; i < elem.count(); i++)
    {
        QString gameName = elem.at(i).firstChildElement("name").text();

        bool alreadyExists = false;
        for (int i = 0; i < dashGPD->gamesPlayed.size(); i++)
            if (dashGPD->gamesPlayed.at(i).gameName == gameName.toStdWString())
            {
                alreadyExists = true;
                break;
            }

        if (alreadyExists)
            continue;

        QString titleId = elem.at(i).firstChildElement("titleid").text();
        QString achievementCount = elem.at(i).firstChildElement("achievementcount").text();
        QString totalGamerscore = elem.at(i).firstChildElement("totalgamerscore").text();
        QString totalAwardCount = elem.at(i).firstChildElement("totalavatarawardcount").text();
        QString maleAwardCount = elem.at(i).firstChildElement("maleavatarawardcount").text();
        QString femaleAwardCount = elem.at(i).firstChildElement("femaleavatarawardcount").text();

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetAllGames);
        item->setText(0, gameName);
        item->setText(1, totalGamerscore);
        item->setText(2, totalAwardCount);

        TitleEntry entry;
        entry.gameName = gameName.toStdWString();
        entry.titleID = titleId.toULong(0, 16);
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

void GameAdderDialog::finishedDownloadingGPD(QString gamePath, QString awardPath, TitleEntry entry)
{
    if (!allowInjection)
    {
        QFile::remove(gamePath);

        if (!awardPath.isEmpty())
            QFile::remove(awardPath);

        return;
    }

    try
    {
        QString gpdName = QString::number(entry.titleID, 16).toUpper() + ".gpd";

        // inject the game gpd
        package->InjectFile(gamePath.toStdString(), gpdName.toStdString());
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
                    package->ExtractFile("PEC", pecTempPath.toStdString());
                    existed = true;
                }

                pecPackage = new StfsPackage(pecTempPath.toStdString(), flags);
            }

            // inject the gpd and delete it
            pecPackage->InjectFile(awardPath.toStdString(), gpdName.toStdString());
            QFile::remove(awardPath);
        }

        // update the dash gpd
        dashGPD->CreateTitleEntry(&entry);
        dashGPD->gamePlayedCount.int32++;
    }
    catch (std::string error)
    {
        QMessageBox::critical(this, "Error Occured", "An error occured while injecting the game.\n\n" + QString::fromStdString(error));
    }
    catch(...)
    {
        QMessageBox::critical(this, "Error Occured", "An unknown error occured while injecting the game(s).");
    }

    if (downloadedCount++ == totalDownloadCount)
    {
        try
        {
            dashGPD->WriteSettingEntry(dashGPD->gamePlayedCount);
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error Writing Entry", "The entry was not written successfully.\n\n" + QString::fromStdString(error));
            close();
        }
        dashGPD->Close();

        try
        {
            package->ReplaceFile(dashGPDTempPath.toStdString(), "FFFE07D1.gpd");
            QFile::remove(dashGPDTempPath);
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error Replacing GPD", "The dashboard GPD could not be replaced.\n\n" + QString::fromStdString(error));
            close();
        }

        std::string kvPath = QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType);
        try
        {
            if (pecPackage != NULL)
            {
                pecPackage->Rehash();
                pecPackage->Resign(kvPath);

                pecPackage->Close();

                if (!existed)
                    package->InjectFile(pecTempPath.toStdString(), "PEC");
                else
                    package->ReplaceFile(pecTempPath.toStdString(), "PEC");

                QFile::remove(pecTempPath);
            }
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "PEC Error" , "The PEC file could not be replaced/injected.\n\n" + QString::fromStdString(error));
            close();
        }

        try
        {
            package->Rehash();
            package->Resign(kvPath);
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

void GameAdderDialog::on_treeWidgetAllGames_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current == NULL)
    {
        ui->lblGameName->setText("Game Name: <i>N/A</i>");
        ui->lblTitleID->setText("Title ID: <i>N/A</i>");
        ui->imgThumbnail->setPixmap(QPixmap());
        return;
    }

    TitleEntry entry = current->data(0, Qt::UserRole).value<TitleEntry>();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(thumbnailReplyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("http://image.xboxlive.com/global/t." + QString::number(entry.titleID, 16) + "/icon/0/8000")));

    ui->lblGameName->setText("Game Name: " + QString::fromStdWString(entry.gameName));
    ui->lblTitleID->setText("Title ID: " + QString::number(entry.titleID, 16).toUpper());
}

void GameAdderDialog::on_pushButton_clicked()
{
    close();
}

void GameAdderDialog::on_pushButton_2_clicked()
{
    int totalCount = ui->treeWidgetQueue->topLevelItemCount();
    if (totalCount < 1)
        return;

    totalDownloadCount = totalCount;
    downloadedCount = 1;

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

        GPDDownloader *downloader = new GPDDownloader(entry, entry.avatarAwardCount != 0, this);
        connect(downloader, SIGNAL(FinishedDownloading(QString, QString, TitleEntry)), this, SLOT(finishedDownloadingGPD(QString, QString, TitleEntry)));
        downloader->BeginDownload();
    }

    if (totalCount == 0)
        this->close();
}
