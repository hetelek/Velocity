#include "titleidfinder.h"

TitleIdFinder::TitleIdFinder(QString gameName, QObject *parent) : QObject(parent), gameName(gameName)
{

}

void TitleIdFinder::StartSearch()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    QUrl url("http://marketplace.xbox.com/en-US/Search?query=" + gameName + "&DownloadType=Game");
    manager->get(QNetworkRequest(url));
}

void TitleIdFinder::replyFinished(QNetworkReply *reply)
{
    QString pageSource(reply->readAll());
    QRegExp exp("66acd000-77fe-1000-9115-d802(.{8})\\?cid=search\"\\>([^<]*)");

    QList<TitleData> matches;

    int pos = 0;
    while ((pos = exp.indexIn(pageSource, pos)) != -1)
    {
        TitleData t;
        t.titleID = exp.cap(1).toULong(0, 16);
        t.titleName = exp.cap(2);
        pos += exp.matchedLength();

        if (t.titleName != "Learn More")
            matches.push_back(t);
    }

    reply->deleteLater();

    emit SearchFinished(matches);
}

void TitleIdFinder::SetGameName(QString gameName)
{
    this->gameName = gameName;
}
