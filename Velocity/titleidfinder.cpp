#include "titleidfinder.h"

TitleIdFinder::TitleIdFinder(QString gameName, QObject *parent) : QObject(parent),
    gameName(gameName)
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
    QRegularExpression exp("66acd000-77fe-1000-9115-d802(.{8})\\?cid=search\"\\>([^<]*)");

    QList<TitleData> matches;

    auto matchIterator = exp.globalMatch(pageSource);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        TitleData t;
        t.titleID = match.captured(1).toULong(0, 16);
        t.titleName = match.captured(2);

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
