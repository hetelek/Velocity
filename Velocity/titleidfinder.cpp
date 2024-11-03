#include "titleidfinder.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

TitleIdFinder::TitleIdFinder(QString gameName, QObject* parent) : QObject(parent), gameName(gameName) {
    loadGameDatabase();
    QMetaObject::invokeMethod(this, "replyFinished", Qt::QueuedConnection); // Trigger replyFinished
}

void TitleIdFinder::loadGameDatabase() {
    QFile file("gamedatabase.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return; // Exit if the file can't be opened
    }

    QByteArray jsonData = file.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject rootObject = jsonDoc.object();
    QJsonArray gamesArray = rootObject["games"].toArray();

    for (const QJsonValue& value : gamesArray) {
        QJsonObject obj = value.toObject();
        QString name = obj["nm"].toString();
        DWORD id = obj["tid"].toString().toUInt(nullptr, 16); // Parse as hex

        TitleData data;
        data.titleName = name;
        data.titleID = id;

        nameToData.insert(name, data); // Store data for name-based lookup
    }
}

void TitleIdFinder::StartSearch() {
    QList<TitleData> matches;

    // Iterate over each entry in nameToData for partial and case-insensitive matches
    for (auto it = nameToData.begin(); it != nameToData.end(); ++it) {
        if (it.key().contains(gameName, Qt::CaseInsensitive)) {
            matches.append(it.value());
        }
    }

    emit SearchFinished(matches);
}

void TitleIdFinder::SetGameName(QString newGameName) {
    gameName = newGameName;
}

void TitleIdFinder::replyFinished(QNetworkReply* /*reply*/) {
    // Process data using the loaded JSON database as if it were a network response
    StartSearch(); // Re-use the StartSearch method for compatibility
}
