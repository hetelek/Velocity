#include "gamerpicturepackdialog.h"
#include "ui_gamerpicturepackdialog.h"

GamerPicturePackDialog::GamerPicturePackDialog(QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::GamerPicturePackDialog), statusBar(statusBar)
{
    ui->setupUi(this);

    titleIDFinder = new TitleIdFinder("", this);
    connect(titleIDFinder, SIGNAL(SearchFinished(QList<Title>)), this, SLOT(onTitleIDSearchReturn(QList<Title>)));

    searchedIDs = new QList<QString>;

    downloader = new GamerPictureDownloader(this);
    connect(downloader, SIGNAL(GamerPictureDownloaded(QPixmap,QString)), this, SLOT(gamerPictureDownloaded(QPixmap,QString)));
}

GamerPicturePackDialog::~GamerPicturePackDialog()
{
    delete searchedIDs;
    delete ui;
}

void GamerPicturePackDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->txtSearch->setPlaceholderText(arg1);
}

void GamerPicturePackDialog::on_pushButton_clicked()
{
    // search by game name
    if (ui->comboBox->currentIndex() == 0)
    {
        // make sure there's a game name
        if (ui->txtSearch->text() == "")
            return;

        statusBar->showMessage("Searching for '" + ui->txtSearch->text() + "'...", 3000);

        // set the title name
        titleIDFinder->SetGameName(ui->txtSearch->text());
        titleIDFinder->StartSearch();
    }
}

void GamerPicturePackDialog::onTitleIDSearchReturn(QList<Title> titlesFound)
{
    if (titlesFound.size() == 0)
        return;

    statusBar->showMessage("Search returned " + QString::number(titlesFound.size()) + " result(s)", 3000);

    currentTitles = new QList<Title>(titlesFound);

    // clear the current names
    ui->listGameNames->clear();

    // display all the titles found in the widget
    for (DWORD i = 0; i < titlesFound.size(); i++)
        ui->listGameNames->addItem(titlesFound.at(i).titleName.replace("&#174;", "®").replace("&#39;", "'").replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("â", "").replace("¢", ""));
}

void GamerPicturePackDialog::on_listGameNames_itemClicked(QListWidgetItem *item)
{
    findGamerPictures(QString::number(currentTitles->at(ui->listGameNames->currentIndex().row()).titleID, 16).toUpper());
}

void GamerPicturePackDialog::findGamerPictures(QString titleID)
{
    statusBar->showMessage("Searching for gamerpictures....", 0x7FFFFFFF);

    downloader->SetTitleID(titleID);
    downloader->start();
}

void GamerPicturePackDialog::gamerPictureDownloaded(QPixmap picture, QString id)
{
    QListWidgetItem *item = new QListWidgetItem(ui->listSearch);
    item->setIcon(QIcon(picture));
    ui->listSearch->addItem(item);

    searchedIDs->push_back(id);
    QApplication::processEvents();

    //QMessageBox::information(this, "", "yo i found one dawg");
}
