#ifndef BOXARTRETRIEVER_H
#define BOXARTRETRIEVER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QPixmap>

#include "winnames.h"

enum BoxArtSize
{
    BoxArtLarge,
    BoxArtSmall
};

class BoxArtRetriever : public QObject
{
    Q_OBJECT
public:
    explicit BoxArtRetriever(QObject *parent = 0);

    void FetchBoxArt(DWORD titleID, BoxArtSize size);

signals:
    void largeBoxArtRetrieved(QPixmap boxart);

    void smallBoxArtRetrieved(QPixmap boxart);

public slots:
    void onNetworkManagerFinished(QNetworkReply*);

private:
    QNetworkAccessManager *manager;
};

#endif // BOXARTRETRIEVER_H
