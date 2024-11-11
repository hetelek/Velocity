#ifndef GAMEADDERDIALOG_H
#define GAMEADDERDIALOG_H

#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkInformation>
#include <QNetworkReply>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QMenu>
#include <QMessageBox>
#include <QStringList>
#include <QMutex>
#include <memory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

// Other includes
#include "Gpd/DashboardGpd.h"
#include "gpddownloader.h"
#include "Stfs/StfsPackage.h"
#include "qthelpers.h"

Q_DECLARE_METATYPE(TitleEntry)

namespace Ui {
class GameAdderDialog;
}

class GameAdderDialog : public QDialog {
    Q_OBJECT

public:
    explicit GameAdderDialog(StfsPackage* package, QWidget* parent = nullptr, bool dispose = true, bool* ok = nullptr);
    ~GameAdderDialog();

private slots:
    void on_treeWidgetAllGames_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void gameReplyFinished(QNetworkReply* reply);
    void showRemoveContextMenu_QueuedGames(QPoint point);
    void showRemoveContextMenu_AllGames(QPoint point);
    void finishedDownloadingGpd(QString gamePath, QString awardPath, TitleEntry entry, bool error);
    void thumbnailReplyFinished(QNetworkReply* reply);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_btnShowAll_clicked();
    void on_txtSearch_textChanged(const QString& arg1);
    void on_btnShowAll_2_clicked(bool checked);

private:
    void initializePecPackage();
    void finalizeInjection();
    void showAllItems();
    void getListing();

    std::unique_ptr<Ui::GameAdderDialog> ui;
    std::unique_ptr<QNetworkAccessManager> manager;
    std::unique_ptr<QNetworkAccessManager> imageManager;
    std::unique_ptr<DashboardGpd> dashGpd;
    std::unique_ptr<StfsPackage> pecPackage;

    QString dashGpdTempPath;
    QString pecTempPath;
    QStringList notSuccessful;

    StfsPackage* package;
    bool existed = false;
    bool allowInjection = false;
    bool dispose = true;

    int totalDownloadCount = 0;
    int downloadedCount = 0;
    TitleEntry entry;
    QMutex m;
};

#endif // GAMEADDERDIALOG_H
