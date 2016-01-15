#include "boxartretriever.h"

BoxArtRetriever::BoxArtRetriever(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkManagerFinished(QNetworkReply*)));
}

void BoxArtRetriever::FetchBoxArt(DWORD titleID, BoxArtSize size)
{
    QString url;
    QString titleIDStr = QString::number(titleID, 16).toLower();
    switch (size)
    {
        case BoxArtLarge:
            url = "http://download.xbox.com/content/images/66acd000-77fe-1000-9115-d802" + titleIDStr + "/1033/boxartlg.jpg";
            break;
        default:
            url = "http://download.xbox.com/content/images/66acd000-77fe-1000-9115-d802" + titleIDStr + "/1033/boxartsm.jpg";
            break;
    }

    manager->get(QNetworkRequest(QUrl(url)));
}

void BoxArtRetriever::onNetworkManagerFinished(QNetworkReply *reply)
{
    QPixmap boxArt;
    if (reply->isReadable())
    {
        QByteArray buffer = reply->readAll();
        boxArt = QPixmap::fromImage(QImage::fromData(buffer));
    }

    // the url must be checked because if we just stored it as a member variable the user could call FetchBoxArt
    // again for the other size and change it before the server responds
    if (reply->url().toString().contains("boxartlg"))
        emit largeBoxArtRetrieved(boxArt);
    else
        emit smallBoxArtRetrieved(boxArt);
}
