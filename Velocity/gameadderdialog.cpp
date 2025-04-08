#include "gameadderdialog.h"
#include "ui_gameadderdialog.h"

GameAdderDialog::GameAdderDialog(StfsPackage *package, QWidget *parent, bool dispose, bool *ok)
    : QDialog(parent), ui(std::make_unique<Ui::GameAdderDialog>()), package(package), dispose(dispose) {

    // Set up the dialog window
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    // Configure the font for the game name label
    QFont font = ui->lblGameName->font();
#ifdef _WIN32
    font.setPointSize(10);
#endif
    ui->lblGameName->setFont(font);

    // Initialize member variables
    allowInjection = false;
    pecPackage = nullptr;

    // Set up the headers of the tree widgets
    ui->treeWidgetAllGames->header()->resizeSection(0, 182);
    ui->treeWidgetQueue->header()->resizeSection(0, 182);

    // Check if the temporary directory is valid and writable
    QDir tempDir(QDir::tempPath());
    QFileInfo tempDirInfo(tempDir.absolutePath());
    if (!tempDir.exists() || !tempDirInfo.isWritable()) {
        QMessageBox::critical(this, "Path Error", "The temporary path is not valid or not writable.");
        close();
        return;
    }

    // Extract and initialize the dashboard GPD
    try {
        dashGpdTempPath = QDir::cleanPath(tempDir.path() + "/" + QUuid::createUuid().toString().remove('{').remove('}').remove('-'));
        package->ExtractFile("FFFE07D1.gpd", dashGpdTempPath.toStdString());
        dashGpd = std::make_unique<DashboardGpd>(dashGpdTempPath.toStdString());
    } catch (const std::string &error) {
        QMessageBox::critical(this, "File Error", "The dashboard Gpd could not be extracted/opened.\n\n" + QString::fromStdString(error));
        close();
        return;
    }

    // Generate and clean the PEC temporary path
    pecTempPath = QDir::cleanPath(tempDir.path() + "/" + QUuid::createUuid().toString().remove('{').remove('}').remove('-'));

    // Initialize the network manager
    manager = std::make_unique<QNetworkAccessManager>(this);

    // Check for network reachability
    if (QNetworkInformation::instance()->reachability() != QNetworkInformation::Reachability::Online) {
        QMessageBox::warning(this, "Listing Error", "The listing could not be parsed. Try again later, as the servers may be down and make sure Velocity has access to the internet.");
        *ok = false;
        return;
    }

    // Connect network signals and initialize network operations
    connect(manager.get(), &QNetworkAccessManager::finished, this, &GameAdderDialog::gameReplyFinished);
    getListing();

    // Set up the context menus for the tree widgets
    ui->treeWidgetAllGames->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidgetQueue->setContextMenuPolicy(Qt::CustomContextMenu);

    // Set up the image manager for handling thumbnails
    imageManager = std::make_unique<QNetworkAccessManager>(this);
    connect(imageManager.get(), &QNetworkAccessManager::finished, this, &GameAdderDialog::thumbnailReplyFinished);

    // Connect custom context menu signals to their respective slots
    connect(ui->treeWidgetAllGames, &QWidget::customContextMenuRequested, this, &GameAdderDialog::showRemoveContextMenu_AllGames);
    connect(ui->treeWidgetQueue, &QWidget::customContextMenuRequested, this, &GameAdderDialog::showRemoveContextMenu_QueuedGames);

    *ok = true;
}
GameAdderDialog::~GameAdderDialog() = default;

void GameAdderDialog::gameReplyFinished(QNetworkReply *aReply) {
    // Check for network errors
    if (aReply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Network Error",
                              "Failed to fetch game data. Error: " + aReply->errorString());
        aReply->deleteLater();
        return;
    }

    // Read the JSON data from the reply
    QString jsonStr = aReply->readAll();

    // Get the games played from dashGpd
    std::vector<TitleEntry> gamesPlayed = dashGpd->gamesPlayed;

    // Parse the JSON data using Qt's built-in JSON functionality
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        ui->tabWidget->setEnabled(false);
        QMessageBox::warning(this, "Listing Error",
                             "The listing could not be parsed. Try again later, as the servers may be down and make sure Velocity has access to the internet.");
        return;
    }

    // Extract the main JSON object and the "games" array
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray gamesArray = jsonObj.value("games").toArray();

    // Iterate through each game in the "games" array
    for (const QJsonValue &gameValue : gamesArray) {
        QJsonObject gameMap = gameValue.toObject();
        QString gameName = gameMap.value("nm").toString();

        // Extract and convert other fields, ensuring we use correct conversions
        DWORD titleId = gameMap.value("tid").toString().toULong(nullptr, 16);

        bool alreadyExists = false;
        for (size_t i = 0; i < gamesPlayed.size(); ++i) {
            if (gamesPlayed[i].titleID == titleId) {
                gamesPlayed.erase(gamesPlayed.begin() + i);
                alreadyExists = true;
                break;
            }
        }

        if (alreadyExists) continue;

        // Correctly convert all fields to ensure they are populated
        DWORD achievementCount = static_cast<DWORD>(gameMap.value("achc").toInt());
        DWORD totalGamerscore = static_cast<DWORD>(gameMap.value("ttlgs").toInt());
        BYTE totalAwardCount = static_cast<BYTE>(gameMap.value("ttlac").toInt());
        BYTE maleAwardCount = static_cast<BYTE>(gameMap.value("ttlmac").toInt());
        BYTE femaleAwardCount = static_cast<BYTE>(gameMap.value("ttlfac").toInt());

        // Create a new item for the tree widget
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetAllGames);
        item->setText(0, gameName.trimmed());
        item->setText(1, QString::number(totalGamerscore));
        item->setText(2, QString::number(totalAwardCount));

        // Populate the TitleEntry structure
        TitleEntry entry = {
            .entry = {
                .type = Undefined,
                .id = 0,
                .addressSpecifier = 0,
                .length = 0
            },
            .initialLength = 0,
            .titleID = titleId,
            .achievementCount = achievementCount,
            .achievementsUnlocked = 0,
            .totalGamerscore = totalGamerscore,
            .gamerscoreUnlocked = 0,
            .achievementsUnlockedOnline = 0,
            .avatarAwardsEarned = 0,
            .avatarAwardCount = totalAwardCount,
            .maleAvatarAwardsEarned = 0,
            .maleAvatarAwardCount = maleAwardCount,
            .femaleAvatarAwardsEarned = 0,
            .femaleAvatarAwardCount = femaleAwardCount,
            .flags = 0,
            .lastPlayed = QDateTime::currentSecsSinceEpoch(),
            .gameName = gameName.toStdWString()
        };

        item->setData(0, Qt::UserRole, QVariant::fromValue(entry));
        ui->treeWidgetAllGames->addTopLevelItem(item);
    }

    // Set the first item as selected and sort the list alphabetically
    if (ui->treeWidgetAllGames->topLevelItemCount() > 0) {
        ui->treeWidgetAllGames->setCurrentItem(ui->treeWidgetAllGames->topLevelItem(0));
        ui->treeWidgetAllGames->topLevelItem(0)->setSelected(true);
    }
    ui->treeWidgetAllGames->sortByColumn(0, Qt::AscendingOrder);
    ui->tabWidget->setEnabled(true);

    aReply->deleteLater();
}




void GameAdderDialog::thumbnailReplyFinished(QNetworkReply *aReply) {
    // Check for network errors
    if (aReply->error() != QNetworkReply::NoError) {
        ui->imgThumbnail->setText("<i>Unable to download image. Error: " + aReply->errorString() + "</i>");
        aReply->deleteLater();
        return;
    }

    int count = ui->treeWidgetAllGames->selectedItems().count();
    if (count != 1) {
        ui->imgThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
    } else {
        QByteArray thumbnail = aReply->readAll();
        if (!thumbnail.isEmpty() && !thumbnail.contains("File not found.")) {
            ui->imgThumbnail->setPixmap(QPixmap::fromImage(QImage::fromData(thumbnail)));
        } else {
            ui->imgThumbnail->setText("<i>Unable to download image.</i>");
        }
    }
    aReply->deleteLater();
}

void GameAdderDialog::showRemoveContextMenu_AllGames(QPoint point) {
    if (ui->treeWidgetAllGames->selectedItems().isEmpty()) return;

    QMenu contextMenu;
    contextMenu.addAction(QPixmap(":/Images/add.png"), "Add to Queue");
    auto selectedItem = contextMenu.exec(ui->treeWidgetAllGames->mapToGlobal(point));
    if (!selectedItem) return;

    for (auto *item : ui->treeWidgetAllGames->selectedItems()) {
        auto queueItem = std::make_unique<QTreeWidgetItem>(ui->treeWidgetQueue);
        queueItem->setText(0, item->text(0));
        queueItem->setText(1, item->text(1));
        queueItem->setText(2, item->text(2));
        queueItem->setData(0, Qt::UserRole, item->data(0, Qt::UserRole));
        ui->treeWidgetQueue->addTopLevelItem(queueItem.release());

        allowInjection = true;
        delete item;
    }

    ui->lblAchievements->setText("N/A");
    ui->lblAvatarAwards->setText("N/A");
    ui->lblGameName->setText("");
    ui->lblGamerscore->setText("N/A");
    ui->lblTitleID->setText("");
    ui->imgThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
}

void GameAdderDialog::finishedDownloadingGpd(QString gamePath, QString awardPath, TitleEntry entry, bool error) {
    auto *downloader = qobject_cast<GpdDownloader *>(sender());
    int index = downloader ? downloader->index() : -1;
    delete downloader;

    // Update the progress bar safely using QMetaObject::invokeMethod
    QMetaObject::invokeMethod(ui->progressBar, "setValue", Qt::QueuedConnection,
                              Q_ARG(int, static_cast<int>(((double)++downloadedCount / totalDownloadCount) * 100)));

    if (error || !allowInjection) {
        if (!error) QFile::remove(gamePath);
        if (!error && !awardPath.isEmpty()) QFile::remove(awardPath);
        if (error) notSuccessful.append(QString::fromStdWString(entry.gameName));
    } else {
        try {
            QMutexLocker locker(&m);
            package->InjectFile(gamePath.toStdString(), QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd");
            QFile::remove(gamePath);

            if (!awardPath.isEmpty()) {
                if (!pecPackage) initializePecPackage();
                pecPackage->InjectFile(awardPath.toStdString(), QString::number(entry.titleID, 16).toUpper().toStdString() + ".gpd");
                QFile::remove(awardPath);
            }

            dashGpd->CreateTitleEntry(&entry);
            dashGpd->gamePlayedCount.int32++;

            // Update the tree widget safely using QMetaObject::invokeMethod
            if (index >= 0) {
                QMetaObject::invokeMethod(ui->treeWidgetQueue, [this, index, entry]() {
                    ui->treeWidgetQueue->topLevelItem(index)->setData(0, Qt::UserRole, QVariant::fromValue(entry));
                }, Qt::QueuedConnection);
            }
        }
        catch (const std::ios_base::failure &e) {
            QMessageBox::critical(this, "File I/O Error", QString("I/O Error: %1").arg(e.what()));
        }
        catch (const std::out_of_range &e) {
            QMessageBox::critical(this, "Out of Range Error", "A data block or memory access was out of range.");
        }
        catch (const std::runtime_error &e) {
            QMessageBox::critical(this, "Runtime Error", QString("Runtime Error: %1").arg(e.what()));
        }
        catch (const std::invalid_argument &e) {
            QMessageBox::critical(this, "Invalid Argument Error", QString("Invalid Argument: %1").arg(e.what()));
        }
        catch (const std::string &error) {
            QMessageBox::critical(this, "String Error", QString::fromStdString(error));
        }
        catch (const std::exception &e) {
            QMessageBox::critical(this, "Standard Exception", QString("Error: %1").arg(e.what()));
        }
        catch (...) {
            QMessageBox::critical(this, "Unknown Error", "An unknown error occurred during the file injection process.");
        }
    }

    if (downloadedCount == totalDownloadCount) {
        finalizeInjection();
    }
}

void GameAdderDialog::initializePecPackage() {
    try {
        DWORD flags = StfsPackagePEC;

        // Check if the PEC file exists in the package
        if (!package->FileExists("PEC")) {
            flags |= StfsPackageCreate;
            existed = false;
        } else {
            try {
                StfsFileEntry pecEntry = package->GetFileEntry("PEC");

                // Simplified validation for block counts
                if (pecEntry.blocksForFile < 3) {
                    QMessageBox::warning(this, "Invalid PEC File", "The PEC file has an invalid block count and may be corrupted.");
                    flags |= StfsPackageCreate;
                    existed = false;  // Treat the file as non-existent to recreate it
                } else {
                    QMutexLocker locker(&m);
                    package->ExtractFile("PEC", pecTempPath.toStdString());
                    existed = true;
                }
            }
            catch (const std::string &error) {
                QMessageBox::critical(this, "File Entry Error", "Error retrieving PEC file entry:\n" + QString::fromStdString(error));
                return;
            }
        }

        pecPackage = std::make_unique<StfsPackage>(pecTempPath.toStdString(), flags);
    }
    catch (const std::ios_base::failure &e) {
        QMessageBox::critical(this, "File I/O Error", QString("I/O Error: %1").arg(e.what()));
    }
    catch (const std::out_of_range &e) {
        QMessageBox::critical(this, "Out of Range Error", "Attempted to access an invalid file entry.");
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(this, "Runtime Error", QString("Runtime Error: %1").arg(e.what()));
    }
    catch (const std::bad_alloc &e) {
        QMessageBox::critical(this, "Memory Allocation Error", "Failed to allocate memory for PEC package.");
    }
    catch (const std::exception &e) {
        QMessageBox::critical(this, "Standard Exception", QString("Error: %1").arg(e.what()));
    }
    catch (...) {
        QMessageBox::critical(this, "Unknown Error", "An unknown error occurred during PEC package initialization.");
    }
}

void GameAdderDialog::finalizeInjection() {
    try {
        if (!notSuccessful.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Some games failed to add:\n" + notSuccessful.join("\n"));
        }

        // Ensure the settings entry is written to the GPD
        try {
            dashGpd->WriteSettingEntry(dashGpd->gamePlayedCount);
        }
        catch (const std::string &error) {
            QMessageBox::critical(this, "GPD Write Error", "Error writing setting entry to GPD:\n" + QString::fromStdString(error));
        }

        // Rehash and resign the package
        package->Rehash();
        package->Resign(QtHelpers::GetKVPath(package->metaData->certificate.ownerConsoleType));

        // Handle cleanup if necessary
        if (dispose) {
            try {
                if (package) {
                    package->Close();
                    delete package;
                    package = nullptr;
                }
            } catch (const std::exception &e) {
                QMessageBox::critical(this, "Cleanup Error", QString("Failed to close package: %1").arg(e.what()));
            }

            if (!QFile::remove(dashGpdTempPath)) {
                QMessageBox::warning(this, "File Deletion Warning", "Failed to delete temporary dashboard GPD file.");
            }

            if (pecPackage) {
                try {
                    pecPackage->Close();
                    pecPackage.reset();
                } catch (const std::exception &e) {
                    QMessageBox::critical(this, "Cleanup Error", QString("Failed to close PEC package: %1").arg(e.what()));
                }
            }

            if (!QFile::remove(pecTempPath)) {
                QMessageBox::warning(this, "File Deletion Warning", "Failed to delete temporary PEC file.");
            }
        }

        close();
    }
    catch (const std::ios_base::failure &e) {
        QMessageBox::critical(this, "File I/O Error", QString("I/O Error: %1").arg(e.what()));
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(this, "Runtime Error", QString("Runtime Error: %1").arg(e.what()));
    }
    catch (const std::bad_alloc &e) {
        QMessageBox::critical(this, "Memory Allocation Error", "Memory allocation failed.");
    }
    catch (const std::string &error) {
        QMessageBox::critical(this, "String Error", QString::fromStdString(error));
    }
    catch (const std::exception &e) {
        QMessageBox::critical(this, "Standard Exception", QString("Error: %1").arg(e.what()));
    }
    catch (...) {
        QMessageBox::critical(this, "Unknown Error", "An unknown error occurred during the finalization process.");
    }
}


void GameAdderDialog::showRemoveContextMenu_QueuedGames(QPoint point) {
    if (ui->treeWidgetQueue->selectedItems().isEmpty()) return;

    QMenu contextMenu;
    contextMenu.addAction(QPixmap(":/Images/delete.png"), "Remove from Queue");
    auto selectedItem = contextMenu.exec(ui->treeWidgetQueue->mapToGlobal(point));
    if (!selectedItem) return;

    for (auto *item : ui->treeWidgetQueue->selectedItems()) {
        auto allGamesItem = std::make_unique<QTreeWidgetItem>(ui->treeWidgetAllGames);
        allGamesItem->setText(0, item->text(0));
        allGamesItem->setText(1, item->text(1));
        allGamesItem->setText(2, item->text(2));
        allGamesItem->setData(0, Qt::UserRole, item->data(0, Qt::UserRole));
        ui->treeWidgetAllGames->addTopLevelItem(allGamesItem.release());

        delete item;
    }
}

void GameAdderDialog::on_treeWidgetAllGames_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    if (!current) {
        ui->lblGameName->setText("<i>N/A</i>");
        ui->lblTitleID->setText("<span style=\"color:#4f4f4f;\"><i>N/A</i></span>");
        ui->imgThumbnail->setPixmap(QPixmap());
        return;
    }

    if (ui->treeWidgetAllGames->selectedItems().count() == 1) {
        TitleEntry entry = current->data(0, Qt::UserRole).value<TitleEntry>();
        imageManager->get(QNetworkRequest(QUrl("http://image.xboxlive.com/global/t." + QString::number(entry.titleID, 16) + "/icon/0/8000")));

        ui->lblGameName->setText(QString::fromStdWString(entry.gameName));
        ui->lblTitleID->setText("<span style=\"color:#4f4f4f;\">" + QString::number(entry.titleID, 16).toUpper() + "</span>");
        ui->lblGamerscore->setText(QString::number(entry.totalGamerscore));
        ui->lblAvatarAwards->setText(QString::number(entry.avatarAwardCount));
        ui->lblAchievements->setText(QString::number(entry.achievementCount));
    } else {
        ui->lblAchievements->setText("N/A");
        ui->lblAvatarAwards->setText("N/A");
        ui->lblGameName->setText("");
        ui->lblGamerscore->setText("N/A");
        ui->lblTitleID->setText("");
        ui->imgThumbnail->setPixmap(QPixmap(":/Images/watermark.png"));
    }
}

void GameAdderDialog::on_pushButton_clicked() {
    if (pecPackage) {
        pecPackage->Close();
        pecPackage.reset();
        QFile::remove(pecTempPath);
    }

    dashGpd->Close();
    QFile::remove(dashGpdTempPath);
    close();
}

void GameAdderDialog::on_pushButton_2_clicked() {
    int totalCount = ui->treeWidgetQueue->topLevelItemCount();
    if (totalCount < 1) {
        QMessageBox::warning(this, "No Games", "You have selected no games to add.");
        return;
    }

    totalDownloadCount = totalCount;
    downloadedCount = 0;

    try {
        if (dashGpd->gamePlayedCount.entry.type == 0) {
            dashGpd->gamePlayedCount.type = Int32;
            dashGpd->gamePlayedCount.int32 = 0;
            dashGpd->CreateSettingEntry(&dashGpd->gamePlayedCount, GamercardTitlesPlayed);
        }
    }
    catch (const std::string &error) {
        QMessageBox::critical(this, "GPD Creation Error", "Error creating gamePlayedCount setting entry:\n" + QString::fromStdString(error));
        return;  // Exit the function if an error occurs
    }

    for (int i = 0; i < totalCount; ++i) {
        TitleEntry entry = ui->treeWidgetQueue->topLevelItem(i)->data(0, Qt::UserRole).value<TitleEntry>();
        QString gpdName = QString::number(entry.titleID, 16).toUpper() + ".gpd";

        if (!package->FileExists(gpdName.toStdString())) {
            try {
                auto *downloader = new GpdDownloader(entry, i, entry.avatarAwardCount != 0, this);
                connect(downloader, &GpdDownloader::FinishedDownloading, this, &GameAdderDialog::finishedDownloadingGpd);
                downloader->BeginDownload();
            }
            catch (const std::exception &e) {
                QMessageBox::critical(this, "Download Error", QString("Error starting download: %1").arg(e.what()));
            }
        } else {
            ++downloadedCount;
        }
    }

    if (downloadedCount == totalDownloadCount) {
        if (dispose) {
            package->Close();
            delete package;
            package = nullptr;
        }
        close();
    }
}

void GameAdderDialog::showAllItems() {
    for (int i = 0; i < ui->treeWidgetAllGames->topLevelItemCount(); ++i) {
        ui->treeWidgetAllGames->topLevelItem(i)->setHidden(false);
    }
}

void GameAdderDialog::getListing() {
    ui->treeWidgetAllGames->clear();
    ui->tabWidget->setEnabled(false);

    QString url = "https://raw.githubusercontent.com/Pandoriaantje/xbox360-gpd-files/main/gameadder.json";
    QNetworkRequest request((QUrl(url)));

    QNetworkReply *reply = manager->get(request);

    // Create a timeout timer
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, [this, reply]() {
        if (reply->isRunning()) {
            reply->abort();
            QMessageBox::warning(this, "Timeout Error", "The network request timed out. Please check your connection.");
        }
    });
    timeoutTimer->start(10000); // 10 seconds timeout

    // Clean up the timer when the request is finished
    connect(reply, &QNetworkReply::finished, timeoutTimer, &QTimer::deleteLater);
}

void GameAdderDialog::on_btnShowAll_clicked() {
    ui->txtSearch->clear();
    ui->btnShowAll_2->setChecked(false);
    showAllItems();
}

void GameAdderDialog::on_txtSearch_textChanged(const QString &) {
    auto itemsMatched = ui->treeWidgetAllGames->findItems(ui->txtSearch->text(), Qt::MatchContains | Qt::MatchRecursive);
    for (int i = 0; i < ui->treeWidgetAllGames->topLevelItemCount(); ++i) {
        ui->treeWidgetAllGames->topLevelItem(i)->setHidden(true);
    }

    if (itemsMatched.isEmpty()) {
        ui->txtSearch->setStyleSheet("color: rgb(255, 1, 1);");
        showAllItems();
        return;
    }

    ui->txtSearch->setStyleSheet("");
    for (auto *item : itemsMatched) {
        for (auto *parent = item->parent(); parent; parent = parent->parent()) {
            parent->setHidden(false);
            parent->setExpanded(true);
        }
        item->setHidden(false);
    }
}

void GameAdderDialog::on_btnShowAll_2_clicked(bool checked) {
    for (int i = 0; i < ui->treeWidgetAllGames->topLevelItemCount(); ++i) {
        TitleEntry entry = ui->treeWidgetAllGames->topLevelItem(i)->data(0, Qt::UserRole).value<TitleEntry>();
        ui->treeWidgetAllGames->topLevelItem(i)->setHidden(checked && entry.avatarAwardCount == 0);
    }
}
