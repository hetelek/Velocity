#include "gpddownloader.h"

const QString GpdDownloader::BASE_GITHUB_URL = "https://raw.githubusercontent.com/Pandoriaantje/xbox360-gpd-files/main";

GpdDownloader::GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject *parent)
    : QObject(parent), entry(entry), hasAwards(hasAwards), gpdWritten(false), indexIn(index) {
    gpdDirectory = "/gameadder/";
    networkManager = new QNetworkAccessManager(this);

    // Connect the finished signal to the slot for handling the response
    connect(networkManager, &QNetworkAccessManager::finished, this, &GpdDownloader::onRequestFinished);
}

void GpdDownloader::BeginDownload() {
    QString url = QString("%1/game/%2.gpd")
    .arg(BASE_GITHUB_URL, QString::number(entry.titleID, 16).toUpper());
    QNetworkRequest request((QUrl(url)));
    networkManager->get(request);
}

int GpdDownloader::index() {
    return indexIn;
}

void GpdDownloader::onRequestFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        // Handle network error
        emit FinishedDownloading("", "", entry, true);
        reply->deleteLater();
        return;
    }

    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().remove("{").remove("}").remove("-");

    if (!gpdWritten) {
        gameGpd = tempPath;

        QFile file(tempPath);
        if (file.open(QFile::Truncate | QFile::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
        }

        gpdWritten = true;
        if (hasAwards) {
            // Download the avatar award GPD
            QString awardUrl = QString("%1/award/%2.gpd")
                                   .arg(BASE_GITHUB_URL, QString::number(entry.titleID, 16).toUpper());
            QNetworkRequest awardRequest((QUrl(awardUrl)));
            networkManager->get(awardRequest);
        } else {
            emit FinishedDownloading(gameGpd, "", entry, false);
        }
    } else {
        awardGpd = tempPath;

        QFile file(tempPath);
        if (file.open(QFile::Truncate | QFile::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
        }

        emit FinishedDownloading(gameGpd, awardGpd, entry, false);
    }

    reply->deleteLater();
}

void GpdDownloader::onDone() {
    // Emit the signal indicating that the download is complete
    emit FinishedDownloading(gameGpd, awardGpd, entry, false);
}
