#ifndef GPDDOWNLOADER_H
#define GPDDOWNLOADER_H

#include "Gpd/XdbfDefininitions.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QString>

class GpdDownloader : public QObject {
    Q_OBJECT

public:
    // Constructor
    explicit GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject *parent = nullptr);

    // Start the download process
    void BeginDownload();

    // Get the index of the download
    [[nodiscard]] int index() const noexcept { return indexIn; }

signals:
    void FinishedDownloading(QString gamePath, QString awardPath, TitleEntry entry, bool error);

private slots:
    // Handle the network request completion
    void onRequestFinished(QNetworkReply *reply);

private:
    // Static base URL for GPD files
    static inline const QString BASE_GITHUB_URL = "https://raw.githubusercontent.com/Pandoriaantje/xbox360-gpd-files/main";

    TitleEntry entry;
    int indexIn;
    bool hasAwards;
    bool gpdWritten = false;

    QString gameGpd;
    QString awardGpd;
    QString gpdDirectory = "/gameadder/";

    QNetworkAccessManager networkManager;
};

#endif // GPDDOWNLOADER_H
