#include "titleidfinder.h"

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
    QVariant parsedData = QtJson::Json::parse(jsonData); // Use the Json class within the QtJson namespace

    // Convert parsed data to a QVariantMap and access the "games" array
    QVariantMap rootMap = parsedData.toMap();
    QVariantList gamesArray = rootMap["games"].toList();

    for (const QVariant& game : gamesArray) {
        QVariantMap gameMap = game.toMap();
        QString name = gameMap["nm"].toString();
        DWORD id = gameMap["tid"].toString().toUInt(nullptr, 16); // Parse as hex

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
