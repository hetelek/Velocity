#include "gamerpicturedownloader.h"

GamerPictureDownloader::GamerPictureDownloader(QObject *parent) : QThread(parent)
{
}

void GamerPictureDownloader::SetTitleID(QString titleID)
{
    this->titleID = titleID;
}

void GamerPictureDownloader::start(Priority p)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gamerpictureNetworkReply(QNetworkReply*)));

    amount = 0;
    startDownloads();
}

void GamerPictureDownloader::startDownloads()
{
    const int searchValues[] = {0, 0x400, 0x1000, 0x8000, 0xFFFF};
    for (int x = 0; x < 4; x++)
    {
        int num = searchValues[x] + amount;
        if (num >= searchValues[x + 1])
            continue;

        for (int i = 0; i < MAX_REQUEST_COUNT; i++)
            manager->get(QNetworkRequest(QUrl("http://image.xboxlive.com/global/t." + titleID.toUpper() + "/tile/0/2" + QString("%1").arg(num + i, 4, 16, QChar('0')).toUpper())));
    }
}

void GamerPictureDownloader::gamerpictureNetworkReply(QNetworkReply *reply)
{
    if (reply->bytesAvailable() != 0)
    {
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll(), "PNG");
        if (!pixmap.isNull())
            emit GamerPictureDownloaded(pixmap, reply->url().toString().mid(reply->url().toString().length() - 4));
    }
    reply->deleteLater();


    QMutex m;
    m.lock();

    if (amount++ != 0 && amount % MAX_REQUEST_COUNT == 0)
        startDownloads();

    m.unlock();
}
