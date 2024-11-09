#ifndef GpdDOWNLOADER_H
#define GpdDOWNLOADER_H

// qt
#include <QObject>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

// other
#include "winnames.h"
#include "Gpd/DashboardGpd.h"
#include "IO/FileIO.h"

class GpdDownloader : public QObject {
    Q_OBJECT
public:
    explicit GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject *parent = nullptr);
    void BeginDownload();
    int index();

signals:
    void FinishedDownloading(const QString &gameGpd, const QString &awardGpd, TitleEntry entry, bool error);

private slots:
    void onRequestFinished(QNetworkReply *reply);
    void onDone();

private:
    static const QString BASE_GITHUB_URL;
    TitleEntry entry;
    bool hasAwards;
    bool gpdWritten;
    int indexIn;
    QString gameGpd;
    QString awardGpd;
    QString gpdDirectory;
    QNetworkAccessManager *networkManager;
};

#endif // GPDDOWNLOADER_H
