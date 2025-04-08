#include "titleidfinder.h"

TitleIdFinder::TitleIdFinder(QString gameName, QObject* parent)
    : QObject(parent), gameName(gameName) {
    // Initialize the network manager
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &TitleIdFinder::replyFinished);
}

void TitleIdFinder::StartSearch() {
    // Construct the URL for the API request
    QString url = QString("https://dbox.tools/api/title_ids/?name=%1&system=XBOX360&limit=100&offset=0").arg(gameName);
    QNetworkRequest request((QUrl(url)));
    networkManager->get(request);
}

void TitleIdFinder::replyFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonResponse.object();

        QJsonArray itemsArray = jsonObject["items"].toArray();
        QList<TitleData> matches;

        for (const QJsonValue& value : itemsArray) {
            QJsonObject item = value.toObject();
            TitleData data;
            data.titleName = item["name"].toString();
            data.titleID = item["title_id"].toString().toUInt(nullptr, 16); // Parse as hex, just like the local JSON

            matches.append(data);
        }

        emit SearchFinished(matches);
    } else {
        // Handle errors (optional)
    }
    reply->deleteLater();
}

void TitleIdFinder::SetGameName(QString newGameName) {
    gameName = newGameName;
}
