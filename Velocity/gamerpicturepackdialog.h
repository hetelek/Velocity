#ifndef GAMERPICTUREPACKDIALOG_H
#define GAMERPICTUREPACKDIALOG_H

// qt
#include <QDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QList>
#include <QStatusBar>
#include <QSemaphore>

// other
#include "titleidfinder.h"
#include "gamerpicturedownloader.h"

namespace Ui {
class GamerPicturePackDialog;
}

class GamerPicturePackDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GamerPicturePackDialog(QStatusBar *statusBar, QWidget *parent = 0);
    ~GamerPicturePackDialog();
    
private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_pushButton_clicked();
    void onTitleIDSearchReturn(QList<struct Title>);
    void on_listGameNames_itemClicked(QListWidgetItem *item);
    void gamerPictureDownloaded(QPixmap, QString);

private:
    Ui::GamerPicturePackDialog *ui;
    TitleIdFinder *titleIDFinder;

    QList<struct Title> *currentTitles;
    QList<QString> *searchedIDs;
    QStatusBar *statusBar;
    GamerPictureDownloader *downloader;

    void findGamerPictures(QString titleID);
};

#endif // GAMERPICTUREPACKDIALOG_H
