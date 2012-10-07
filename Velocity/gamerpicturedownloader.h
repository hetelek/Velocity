#ifndef GAMERPICTUREDOWNLOADER_H
#define GAMERPICTUREDOWNLOADER_H

#include <QThread>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QSemaphore>
#include <QPixmap>
#include <QMutex>
#include <QDebug>

#include "winnames.h"

#define MAX_REQUEST_COUNT 16

class GamerPictureDownloader : public QThread
{
    Q_OBJECT
public:
    explicit GamerPictureDownloader(QObject *parent = 0);
    ~GamerPictureDownloader();

    void SetTitleID(QString titleID);
    void start(Priority = InheritPriority);
    
signals:
    void GamerPictureDownloaded(QPixmap picture, QString id);
    
public slots:
    void gamerpictureNetworkReply(QNetworkReply*);

private:
    void startDownloads();

    QString titleID;
    QNetworkAccessManager *manager;

    int amount;
};

#endif // GAMERPICTUREDOWNLOADER_H