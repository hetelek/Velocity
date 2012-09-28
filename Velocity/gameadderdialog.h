#ifndef GAMEADDERDIALOG_H
#define GAMEADDERDIALOG_H

// qt
#include <QDialog>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QMenu>
#include <QtXml/QDomDocument>
#include <QMessageBox>

// other
#include "QtHelpers.h"
#include "GPD/DashboardGPD.h"
#include "gpddownloader.h"
#include "Stfs/StfsPackage.h"

namespace Ui {
class GameAdderDialog;
}

class GameAdderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GameAdderDialog(StfsPackage *package, QWidget *parent = 0);
    ~GameAdderDialog();
    
private slots:
    void on_treeWidgetAllGames_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void gameReplyFinished(QNetworkReply*);
    void showRemoveContextMenu_QueuedGames(QPoint point);
    void showRemoveContextMenu_AllGames(QPoint point);
    void finishedDownloadingGPD(QString gamePath, QString awardPath, TitleEntry entry);
    void thumbnailReplyFinished(QNetworkReply *aReply);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::GameAdderDialog *ui;
    QString mainDir;
    QNetworkAccessManager *manager;
    DashboardGPD *dashGPD;
    QString dashGPDTempPath;
    StfsPackage *package;

    bool allowInjection;
    int totalDownloadCount, downloadedCount;
};

#endif // GAMEADDERDIALOG_H
