#ifndef TITLEIDFINDER_H
#define TITLEIDFINDER_H

// qt
#include <QObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QFile>
#include <QJsonDocument>  // Updated to use Qt's built-in JSON parsing
#include <QJsonObject>    // Updated to use Qt's built-in JSON parsing
#include <QJsonArray>     // Updated to use Qt's built-in JSON parsing

// other
#include "winnames.h"

struct TitleData {
    QString titleName;
    DWORD titleID;
};

class TitleIdFinder : public QObject {
    Q_OBJECT
public:
    explicit TitleIdFinder(QString gameName, QObject *parent = 0);

    void StartSearch();
    void SetGameName(QString gameName);

signals:
    void SearchFinished(QList<TitleData> matches);

public slots:
    void replyFinished(QNetworkReply *reply);

private:
    QString gameName;
    QNetworkAccessManager *networkManager;
    void loadGameDatabase();  // You may remove this if not needed for remote JSON
    QHash<QString, TitleData> nameToData;
};

#endif // TITLEIDFINDER_H
