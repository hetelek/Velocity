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

// other
#include "GPD/DashboardGPD.h"

namespace Ui {
class GameAdderDialog;
}

class GameAdderDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GameAdderDialog(QWidget *parent = 0);
    ~GameAdderDialog();
    
private slots:
    void on_treeWidgetAllGames_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void gameReplyFinished(QNetworkReply*);
    void showRemoveContextMenu_QueuedGames(QPoint point);
    void showRemoveContextMenu_AllGames(QPoint point);
    void thumbnailReplyFinished(QNetworkReply *aReply);

private:
    Ui::GameAdderDialog *ui;
    QString mainDir;
    QNetworkAccessManager *manager;
};

#endif // GAMEADDERDIALOG_H
