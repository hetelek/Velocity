#include "gamerpicturepackdialog.h"
#include "ui_gamerpicturepackdialog.h"

GamerPicturePackDialog::GamerPicturePackDialog(QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::GamerPicturePackDialog), statusBar(statusBar), requestImage(false)
{
    ui->setupUi(this);

    titleIDFinder = new TitleIdFinder("", this);
    connect(titleIDFinder, SIGNAL(SearchFinished(QList<Title>)), this, SLOT(onTitleIDSearchReturn(QList<Title>)));

    searchedIDs = new QList<QString>;
    addedIDs = new QList<QString>;
    searchedTitleIDs = new QList<DWORD>;

    downloader = new GamerPictureDownloader(this);
    connect(downloader, SIGNAL(GamerPictureDownloaded(QImage,QString)), this, SLOT(gamerPictureDownloaded(QImage,QString)));

    ui->listSearch->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listSearch, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuSearch(QPoint)));

    ui->listPack->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listPack, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenuPack(QPoint)));

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamercardNetworkReply(QNetworkReply*)));
}

GamerPicturePackDialog::~GamerPicturePackDialog()
{
    delete searchedIDs;
    delete addedIDs;
    delete searchedTitleIDs;
    delete titleIDFinder;

    downloader->quit();
    delete downloader;

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

        QListWidgetItem *item = new QListWidgetItem(ui->listSearch);
        item->setIcon(QIcon(gamerpic));
        ui->listSearch->addItem(item);

        QString url = reply->url().toString();
        searchedIDs->push_back(url.mid(url.indexOf("t.") + 2, 8) + url.mid(url.length() - 4));

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
        if (searchedTitleIDs->contains(ui->txtSearch->text().toULong(false, 16)))
            return;

        searchedTitleIDs->push_back(ui->txtSearch->text().toULong(false, 16));
        findGamerPictures(ui->txtSearch->text().toUpper());
    }
    else
        manager->get(QNetworkRequest(QUrl("http://gamercard.xbox.com/en-US/" + ui->txtSearch->text() + ".card")));
}

void GamerPicturePackDialog::onTitleIDSearchReturn(QList<struct Title> titlesFound)
{
    if (titlesFound.size() == 0)
        return;

    statusBar->showMessage("Search returned " + QString::number(titlesFound.size()) + " result(s)", 3000);

    currentTitles = new QList<struct Title>(titlesFound);

    // clear the current names
    ui->listGameNames->clear();

    // display all the titles found in the widget
    for (DWORD i = 0; i < titlesFound.size(); i++)
    {
        QString newStr = ((QString*)&titlesFound.at(i).titleName)->replace("&#174;", "®").replace("&#39;", "'").replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("â", "").replace("¢", "");
        ui->listGameNames->addItem(newStr);
    }
}

void GamerPicturePackDialog::on_listGameNames_itemClicked(QListWidgetItem *item)
{
    if (searchedTitleIDs->contains(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID))
        return;

    searchedTitleIDs->push_back(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID);
    findGamerPictures(QString::number(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID, 16).toUpper());
}

void GamerPicturePackDialog::findGamerPictures(QString titleID)
{
    statusBar->showMessage("Searching for gamerpictures....", 0x7FFFFFFF);

    downloader->SetTitleID(titleID);
    downloader->run();
}

void GamerPicturePackDialog::gamerPictureDownloaded(QImage picture, QString id)
{
    QListWidgetItem *item = new QListWidgetItem(ui->listSearch);
    item->setIcon(QIcon(QPixmap::fromImage(picture)));
    ui->listSearch->addItem(item);

    searchedIDs->push_back(id);
    QApplication::processEvents();
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
    if (toAddToStrs->contains(currentStrs->at(current->currentIndex().row())) && current == ui->listSearch)
    {
        statusBar->showMessage("Image already exists in pack", 3000);
        return;
    }
    else if (toAddToStrs->contains(currentStrs->at(current->currentIndex().row())))
    {
        // delete only
        currentStrs->removeAt(current->currentIndex().row());
        delete currentItem;
        return;
    }

    // create a new list item
    QListWidgetItem *newItem = new QListWidgetItem(*currentItem);
    toAddTo->addItem(newItem);

    // move the ID to the pack list
    toAddToStrs->push_back(currentStrs->at(current->currentIndex().row()));
    currentStrs->removeAt(current->currentIndex().row());

    // delete the old item
    delete currentItem;

    ui->btnCreatePack->setEnabled(ui->listPack->count() != 0);
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
                                                        QtHelpers::DesktopLocation() + "/" + getImageName(currentStrs->at(current->currentIndex().row()), true) + ".png", "*.png");

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

        ui->btnCreatePack->setEnabled(ui->listPack->count() != 0);
    }
    else if (selectedItem->text() == "Clear All")
    {
        current->clear();
        currentStrs->clear();
        ui->btnCreatePack->setEnabled(ui->listPack->count() != 0);
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
    QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to save your picture pack", QtHelpers::DesktopLocation() + "\\" + ui->txtPackName->text());
    if (savePath == "")
        return;

    try
    {
        StfsPackage picturePack(savePath.toStdString(), StfsPackageCreate);

        picturePack.metaData->contentType = GamerPicture;
        picturePack.metaData->displayName = ui->txtPackName->text().toStdWString();
        picturePack.metaData->titleID = 0xFFFE07D1;
        picturePack.metaData->titleName = L"Xbox360 Dashboard";

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

        // add all the gamerpictures to the pacakge
        for (DWORD i = 0; i < ui->listPack->count(); i++)
        {
            // inject the 64x64 image
            QByteArray large;
            QBuffer buffLrg(&large);
            buffLrg.open(QIODevice::WriteOnly);
            ui->listPack->item(i)->icon().pixmap(64, 64).save(&buffLrg, "PNG");
            picturePack.InjectData((BYTE*)large.data(), large.length(), getImageName(addedIDs->at(i), true).toStdString() + ".png");

            // inject the 32x32 image
            QByteArray small;
            QBuffer buffSm(&small);
            buffSm.open(QIODevice::WriteOnly);
            ui->listPack->item(i)->icon().pixmap(32, 32).save(&buffSm, "PNG");
            picturePack.InjectData((BYTE*)small.data(), small.length(), getImageName(addedIDs->at(i), false).toStdString() + ".png");
        }

        // fix the package
        picturePack.Rehash();
        picturePack.Resign(QtHelpers::GetKVPath(Retail, this));

        statusBar->showMessage("Successfully created your picture pack", 3000);
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
    for (DWORD i = 1; i < gamertag.length(); i++)
    {
        if (gamertag.at(i) == ' ' && prevChar == ' ')
            return false;
        if (!gamertag.at(i).isLetterOrNumber() && gamertag.at(i) != ' ')
            return false;
        prevChar = gamertag.at(i);
    }
    return true;
}
