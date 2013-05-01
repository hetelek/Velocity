#include "gamerpicturepackdialog.h"
#include "ui_gamerpicturepackdialog.h"

GamerPicturePackDialog::GamerPicturePackDialog(QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::GamerPicturePackDialog), statusBar(statusBar), requestImage(false)
{
    ui->setupUi(this);

    titleIDFinder = new TitleIdFinder("", this);
    connect(titleIDFinder, SIGNAL(SearchFinished(QList<TitleData>)), this, SLOT(onTitleIDSearchReturn(QList<TitleData>)));

    searchedIDs = new QList<QString>;
    addedIDs = new QList<QString>;
    searchedTitleIDs = new QList<DWORD>;

    ui->listSearch->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listSearch, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuSearch(QPoint)));

    ui->listPack->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listPack, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuPack(QPoint)));

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamercardNetworkReply(QNetworkReply*)));

    gpManager = new QNetworkAccessManager(this);
    connect(gpManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamerPictureDownloaded(QNetworkReply*)));

    connect(ui->txtSearch, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));
}

GamerPicturePackDialog::~GamerPicturePackDialog()
{
    delete searchedIDs;
    delete addedIDs;
    delete searchedTitleIDs;

    statusBar->showMessage("");

    delete ui;
}

void GamerPicturePackDialog::gamercardNetworkReply(QNetworkReply *reply)
{
    if (reply->bytesAvailable() == 0)
    {
        if (requestImage)
        {
            searchedGamertags.removeLast();
            statusBar->showMessage("Couldn't download gamertag gamer picture", 3000);
            requestImage = false;
        }
        else
            statusBar->showMessage("Couldn't download gamertag infomation", 3000);
        return;
    }

    if (requestImage)
    {
        requestImage = false;

        QPixmap gamerpic;
        gamerpic.loadFromData(reply->readAll(), "PNG");

        QString url = reply->url().toString();
        QString picID = url.mid(url.indexOf("t.") + 2, 8).toUpper() + "." + url.mid(url.length() - 4).toUpper();

        QListWidgetItem *item = new QListWidgetItem(ui->listSearch);
        item->setIcon(QIcon(gamerpic));
        item->setToolTip("<b>" + currentSearchName + "</b><br />" + picID);
        ui->listSearch->addItem(item);

        searchedIDs->push_back(picID);

        statusBar->clearMessage();
    }
    else
    {
        // setup
        QString pageSource(reply->readAll());
        QRegExp exp("<title>([^<]*).*<img id=\"Gamerpic\" src=\"([^\"]*)");

        exp.indexIn(pageSource, 0);

        // request image
        QString gamertag = exp.cap(1);
        ui->txtSearch->setText(gamertag);

        if (searchedGamertags.contains(gamertag))
            return;

        searchedGamertags.push_back(gamertag);

        QString urlStr = exp.cap(2);
        QUrl url(urlStr);

        if (url.host() == "avatar.xboxlive.com")
        {
            statusBar->showMessage("Cannot add avatar gamer picture", 3000);
            return;
        }

        manager->get(QNetworkRequest(url));
        requestImage = true;


        statusBar->showMessage("Downloading gamer picture...", 1000);
    }
}

void GamerPicturePackDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->txtSearch->setPlaceholderText(arg1);
    ui->txtSearch->setStyleSheet("");
    ui->txtSearch->setText("");

    ui->btnStopSearch->setEnabled(false);

    if (arg1 == "Title ID")
        ui->txtSearch->setMaxLength(8);
    else if (arg1 == "Gamertag")
        ui->txtSearch->setMaxLength(15);
    else
        ui->txtSearch->setMaxLength(64);
}

void GamerPicturePackDialog::on_pushButton_clicked()
{
    // make sure there's something entered
    if (ui->txtSearch->text() == "")
        return;

    // search by game name
    if (ui->comboBox->currentIndex() == 0)
    {
        statusBar->showMessage("Searching for '" + ui->txtSearch->text() + "'...", 3000);

        // set the title name
        titleIDFinder->SetGameName(ui->txtSearch->text());
        titleIDFinder->StartSearch();
    }
    // search by title id
    else if (ui->comboBox->currentIndex() == 1)
    {
        bool ok;
        if (searchedTitleIDs->contains(ui->txtSearch->text().toULong(&ok, 16)))
            return;

        currentSearchName = ui->txtSearch->text();
        searchedTitleIDs->push_back(ui->txtSearch->text().toULong(&ok, 16));
        findGamerPictures(ui->txtSearch->text().toUpper());
    }
    else
    {
        ui->btnStopSearch->setEnabled(false);
        currentSearchName = ui->txtSearch->text();
        manager->get(QNetworkRequest(QUrl("http://gamercard.xbox.com/en-US/" + ui->txtSearch->text() + ".card")));
    }
}

void GamerPicturePackDialog::onTitleIDSearchReturn(QList<TitleData> titlesFound)
{
    if (titlesFound.size() == 0)
        return;

    statusBar->showMessage("Search returned " + QString::number(titlesFound.size()) + " result(s)", 3000);

    currentTitles = new QList<TitleData>(titlesFound);

    // clear the current names
    ui->listGameNames->clear();

    // display all the titles found in the widget
    for (int i = 0; i < titlesFound.size(); i++)
    {
        QString newStr = ((QString*)&titlesFound.at(i).titleName)->replace("&#174;", "®").replace("&#39;", "'").replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("â", "").replace("¢", "");
        ui->listGameNames->addItem(newStr);
    }
}

void GamerPicturePackDialog::on_listGameNames_itemClicked(QListWidgetItem *item)
{
    // make sure that we haven't already searched for the title
    if (searchedTitleIDs->contains(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID))
        return;
    searchedTitleIDs->push_back(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID);

    currentSearchName = item->text();
    findGamerPictures(QString::number(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID, 16));
}

void GamerPicturePackDialog::findGamerPictures(QString titleID)
{
    statusBar->showMessage("Finding gamerpictures for " + titleID.toUpper());
    ui->btnStopSearch->setEnabled(true);

    // offsets to search from
    const int searchOffsets[] = {0, 0x400, 0x1000, 0x8000};

    // reset the manager
    disconnect(gpManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamerPictureDownloaded(QNetworkReply*)));
    delete gpManager;
    gpManager = new QNetworkAccessManager(this);
    connect(gpManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamerPictureDownloaded(QNetworkReply*)));

    // make a bunch of requests
    for (DWORD i = 0; i < 0x400; i++)
    {
        // iterate through all the offsets
        for (int x = 0; x < 4; x++)
            gpManager->get(QNetworkRequest(QUrl("http://image.xboxlive.com/global/t." + titleID.toUpper() + "/tile/0/2" + QString("%1").arg(searchOffsets[x] + i, 4, 16, QChar('0')).toUpper())));
    }
}

void GamerPicturePackDialog::gamerPictureDownloaded(QNetworkReply *reply)
{
    if (reply->bytesAvailable() != 0)
    {
        // parse the image
        QImage image;
        image.loadFromData(reply->readAll(), "PNG");

        // make sure the image is valid
        if (image.isNull())
            return;

        QString pictureID = reply->url().toString().mid(reply->url().toString().indexOf("t.") + 2, 8) + "." + reply->url().toString().mid(reply->url().toString().length() - 4);

        // add the gamerpicture to the list widget
        QListWidgetItem *item = new QListWidgetItem(ui->listSearch);
        item->setIcon(QIcon(QPixmap::fromImage(image)));
        item->setToolTip("<b>" + currentSearchName + "</b><br />" + pictureID);
        ui->listSearch->addItem(item);

        searchedIDs->push_back(pictureID);

        reply->deleteLater();
        QCoreApplication::processEvents();
    }
}

void GamerPicturePackDialog::on_listSearch_itemDoubleClicked(QListWidgetItem *item)
{
    switchImage(item, ui->listSearch, ui->listPack, searchedIDs, addedIDs);
}

void GamerPicturePackDialog::on_listPack_itemDoubleClicked(QListWidgetItem *item)
{
    switchImage(item, ui->listPack, ui->listSearch, addedIDs, searchedIDs);
}

void GamerPicturePackDialog::switchImage(QListWidgetItem *currentItem, QListWidget *current, QListWidget *toAddTo, QList<QString> *currentStrs, QList<QString> *toAddToStrs)
{
    if (toAddToStrs->contains(currentStrs->at(current->row(currentItem))) && current == ui->listSearch)
    {
        statusBar->showMessage("Image already exists in pack", 3000);
        return;
    }
    else if (toAddToStrs->contains(currentStrs->at(current->row(currentItem))))
    {
        // delete only
        currentStrs->removeAt(current->row(currentItem));
        delete currentItem;
        return;
    }

    // create a new list item
    QListWidgetItem *newItem = new QListWidgetItem(*currentItem);
    toAddTo->addItem(newItem);

    // move the ID to the pack list
    toAddToStrs->push_back(currentStrs->at(current->row(currentItem)));
    currentStrs->removeAt(current->row(currentItem));

    // delete the old item
    delete currentItem;

    ui->btnCreatePack->setEnabled(ui->listPack->count() != 0 && ui->txtPackName->text() != "");
}

void GamerPicturePackDialog::showContextMenuSearch(QPoint p)
{
    showContextMenu(p, ui->listSearch, ui->listPack, searchedIDs, addedIDs);
}

void GamerPicturePackDialog::showContextMenuPack(QPoint p)
{
    showContextMenu(p, ui->listPack, ui->listSearch, addedIDs, searchedIDs);
}

void GamerPicturePackDialog::showContextMenu(QPoint p, QListWidget *current, QListWidget *toAddTo, QList<QString> *currentStrs, QList<QString> *toAddToStrs)
{
    QPoint globalPos = current->mapToGlobal(p);
    QMenu contextMenu;

    bool forward = (current == ui->listSearch);

    contextMenu.addAction(QPixmap(":/Images/undo.png"), "Clear All");
    contextMenu.addAction((forward) ? "Add All to Pack" : "Remove All from Pack");

    if (current->selectedItems().count() > 0)
    {
        if (forward)
            contextMenu.addAction(QPixmap(":/Images/add.png"), "Add to Pack");
        else
            contextMenu.addAction(QPixmap(":/Images/delete.png"), "Remove from Pack");
        contextMenu.addAction(QPixmap(":/Images/save.png"), "Save Image");
    }

    QAction *selectedItem = contextMenu.exec(globalPos);
    if (selectedItem == NULL)
        return;

    if (selectedItem->text() == ((forward) ? "Add to Pack" : "Remove from Pack"))
        switchImage(current->currentItem(), current, toAddTo, currentStrs, toAddToStrs);
    else if (selectedItem->text() == "Save Image")
    {
        QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to save the gamerpicture",
                                                        QtHelpers::DesktopLocation() + getImageName(currentStrs->at(current->currentIndex().row()), true) + ".png", "*.png");

        if (savePath == "")
            return;

        current->currentItem()->icon().pixmap(64, 64).save(savePath);

        statusBar->showMessage("Successfully saved gamerpicture", 3000);
    }
    else if (selectedItem->text() == ((forward) ? "Add All to Pack" : "Remove All from Pack"))
    {
        DWORD itemCount = current->count();
        for (DWORD i = 0; i < itemCount; i++)
            switchImage(current->item(0), current, toAddTo, currentStrs, toAddToStrs);

        ui->btnCreatePack->setEnabled(ui->listPack->count() != 0 && ui->txtPackName->text() != "");
    }
    else if (selectedItem->text() == "Clear All")
    {
        current->clear();
        currentStrs->clear();
        ui->btnCreatePack->setEnabled(ui->listPack->count() != 0 && ui->txtPackName->text() != "");
    }
}

QString GamerPicturePackDialog::getImageName(QString id, bool big)
{
    return ((big) ? "64_" : "32_") + id.mid(0, 8) + "0002" + id.mid(8) + "0001" + id.mid(8);
}

void GamerPicturePackDialog::on_txtPackName_textChanged(const QString &arg1)
{
    ui->btnCreatePack->setEnabled(arg1 != "" && ui->listPack->count() != 0);
}

void GamerPicturePackDialog::on_btnCreatePack_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to save your picture pack", QtHelpers::DesktopLocation() + "/" + ui->txtPackName->text());
    if (savePath == "")
        return;

    try
    {
        StfsPackage picturePack(savePath.toStdString(), StfsPackageCreate);

        picturePack.metaData->contentType = GamerPicture;
        std::wstring *w = new std::wstring(ui->txtPackName->text().toStdWString());
        memcpy(&picturePack.metaData->displayName, w, sizeof(std::wstring));
        picturePack.metaData->titleID = 0xFFFE07D1;
        picturePack.metaData->transferFlags = 0x40;

        // set thumbnail image
        QByteArray ba1;
        QBuffer buffer1(&ba1);
        buffer1.open(QIODevice::WriteOnly);
        QPixmap(":/Images/20001.png").save(&buffer1, "PNG");
        picturePack.metaData->thumbnailImage = (BYTE*)ba1.data();
        picturePack.metaData->thumbnailImageSize = ba1.length();

        // set title thumbnail image
        QByteArray ba2;
        QBuffer buffer2(&ba2);
        buffer2.open(QIODevice::WriteOnly);
        QPixmap(":/Images/defaultTitleImage.png").save(&buffer2, "PNG");
        picturePack.metaData->titleThumbnailImage = (BYTE*)ba2.data();
        picturePack.metaData->titleThumbnailImageSize = ba2.length();

        statusBar->showMessage("Creating picture pack, %0 complete");
        ui->tabWidget->setEnabled(false);
        ui->btnCreatePack->setEnabled(false);
        ui->txtPackName->setEnabled(false);
        ui->txtSearch->setEnabled(false);
        ui->btnStopSearch->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->listGameNames->setEnabled(false);
        ui->comboBox->setEnabled(false);

        // add all the gamerpictures to the pacakge
        for (int i = 0; i < ui->listPack->count(); i++)
        {
            // inject the 64x64 image
            QByteArray largeImage;
            QBuffer buffLrg(&largeImage);
            buffLrg.open(QIODevice::WriteOnly);
            ui->listPack->item(i)->icon().pixmap(64, 64).save(&buffLrg, "PNG");
            picturePack.InjectData((BYTE*)largeImage.data(), largeImage.length(), getImageName(addedIDs->at(i), true).toStdString() + ".png");

            QByteArray smallImage;
            QBuffer buffSm(&smallImage);
            buffSm.open(QIODevice::WriteOnly);
            ui->listPack->item(i)->icon().pixmap(32, 32).save(&buffSm, "PNG");
            picturePack.InjectData((BYTE*)smallImage.data(), smallImage.length(), getImageName(addedIDs->at(i), false).toStdString() + ".png");

            statusBar->showMessage("Creating picture pack, " + QString::number(((float)i / (float)ui->listPack->count()) * 100, 'f', 0) + "% complete");
            QApplication::processEvents();
        }

        // fix the package
        picturePack.Rehash();
        picturePack.Resign(QtHelpers::GetKVPath(Retail, this));

        statusBar->showMessage("Successfully created your picture pack", 3000);
        ui->tabWidget->setEnabled(true);
        ui->btnCreatePack->setEnabled(true);
        ui->txtPackName->setEnabled(true);
        ui->txtSearch->setEnabled(true);
        ui->btnStopSearch->setEnabled(true);
        ui->listGameNames->setEnabled(true);
        ui->comboBox->setEnabled(true);
        ui->pushButton->setEnabled(true);
        QMessageBox::information(this, "Success", "Successfully created your picture pack.");
    }
    catch (string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while creating your picture pack.\n\n" + QString::fromStdString(error));
    }
}

void GamerPicturePackDialog::on_txtSearch_textChanged(const QString &arg1)
{
    if (arg1 == "")
    {
        ui->txtSearch->setStyleSheet("");
        ui->pushButton->setEnabled(false);
        return;
    }
    else
        ui->pushButton->setEnabled(true);

    if (ui->comboBox->currentIndex() == 1)
    {
        if (ui->txtSearch->text().length() != 8 || !QtHelpers::VerifyHexString(ui->txtSearch->text()))
        {
            ui->txtSearch->setStyleSheet("color: rgb(255, 1, 1);");
            ui->pushButton->setEnabled(false);
        }
        else
            ui->txtSearch->setStyleSheet("");
    }
    else if (ui->comboBox->currentIndex() == 2)
    {
        if (!verifyGamertag(ui->txtSearch->text()))
        {
            ui->txtSearch->setStyleSheet("color: rgb(255, 1, 1);");
            ui->pushButton->setEnabled(false);
        }
        else
            ui->txtSearch->setStyleSheet("");
    }
}

bool GamerPicturePackDialog::verifyGamertag(QString gamertag)
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

void GamerPicturePackDialog::on_btnStopSearch_clicked()
{
    disconnect(gpManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamerPictureDownloaded(QNetworkReply*)));
    delete gpManager;
    gpManager = new QNetworkAccessManager(this);
    connect(gpManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamerPictureDownloaded(QNetworkReply*)));
    QApplication::processEvents();

    statusBar->showMessage("Search stopped", 3000);
    ui->btnStopSearch->setEnabled(false);
}
