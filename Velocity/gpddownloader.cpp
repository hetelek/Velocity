#include "gpddownloader.h"

// Constructor initializes the GpdDownloader object, setting up the network manager for handling requests
GpdDownloader::GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject *parent)
    : QObject(parent), entry(std::move(entry)), indexIn(index), hasAwards(hasAwards) {
    // Connect the finished signal from the network manager to the onRequestFinished slot
    connect(&networkManager, &QNetworkAccessManager::finished, this, &GpdDownloader::onRequestFinished);
}

// Starts the download process for the game's GPD file
void GpdDownloader::BeginDownload() {
    // Construct the URL to download the game's GPD file from the GitHub repository
    QString url = QString("%1/game/%2.gpd").arg(BASE_GITHUB_URL, QString::number(entry.titleID, 16).toUpper());
    QNetworkRequest request((QUrl(url)));  // Create a network request using the URL
    networkManager.get(request);           // Send the GET request
}

// Slot called when the network request is finished
void GpdDownloader::onRequestFinished(QNetworkReply *reply) {
    // Check for errors in the network reply
    if (reply->error() != QNetworkReply::NoError) {
        // If there's an error, emit the FinishedDownloading signal with an error flag
        emit FinishedDownloading("", "", entry, true);
        reply->deleteLater();  // Clean up the network reply object
        return;
    }

    // Generate a temporary path to save the downloaded GPD file
    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces);

    // Check if the game GPD file has already been written
    if (!gpdWritten) {
        // Save the game GPD file to the temporary path
        gameGpd = tempPath;
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(reply->readAll());  // Write the content of the reply to the file
        }
        file.close();
        gpdWritten = true;  // Mark the GPD file as written

        // If the game has avatar awards, initiate a second download for the award GPD file
        if (hasAwards) {
            QString awardUrl = QString("%1/award/%2.gpd").arg(BASE_GITHUB_URL, QString::number(entry.titleID, 16).toUpper());
            QNetworkRequest awardRequest((QUrl(awardUrl)));
            networkManager.get(awardRequest);  // Send the GET request for the award GPD file
        } else {
            // If there are no awards, emit the FinishedDownloading signal
            emit FinishedDownloading(gameGpd, "", entry, false);
        }
    } else {
        // Save the award GPD file to the temporary path
        awardGpd = tempPath;
        QFile file(tempPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(reply->readAll());  // Write the content of the reply to the file
        }
        file.close();
        // Emit the FinishedDownloading signal with both the game and award GPD paths
        emit FinishedDownloading(gameGpd, awardGpd, entry, false);
    }

    reply->deleteLater();  // Clean up the network reply object
}
