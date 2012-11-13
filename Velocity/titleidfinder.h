#ifndef TITLEIDFINDER_H
#define TITLEIDFINDER_H

// qt
#include <QObject>
#include <QList>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRegExp>

// other
#include "winnames.h"

struct TitleData
{
    QString titleName;
    DWORD titleID;
};

class TitleIdFinder : public QObject
{
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
};

#endif // TITLEIDFINDER_H
