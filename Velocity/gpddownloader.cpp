#include "gpddownloader.h"

GPDDownloader::GPDDownloader(TitleEntry entry, int index, bool hasAwards, QObject * /* parent */ ) : entry(entry), indexIn(index), hasAwards(hasAwards)
{
    gpdDirectory = "/gameadder/";
    gpdWritten = false;

    http = new QHttp(this);
    http->setHost("velocity.expetelek.com");

    connect(http, SIGNAL(done(bool)), this, SLOT(onDone(bool)));
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(onRequestFinished(int, bool)));
}

void GPDDownloader::BeginDownload()
{
    QString url = gpdDirectory + "game/" + QString::number(entry.titleID, 16).toUpper() + ".gpd";
    http->get(url);
}

int GPDDownloader::index()
{
    return indexIn;
}

void GPDDownloader::onRequestFinished(int /* id */, bool error)
{
    if (error)
        qDebug() << http->errorString();
    else if (http->bytesAvailable() < 1)
        return;

    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

    if (!gpdWritten)
    {
        gameGPD = tempPath;

        // create a new temporary file
        QFile v1File(tempPath);
        v1File.open(QFile::Truncate | QFile::WriteOnly);

        // write the gpd to disk
        v1File.write(http->readAll());

        // clean up
        v1File.flush();
        v1File.close();

        gpdWritten = true;
    }
    else
    {
        awardGPD = tempPath;

        // create a new temporary file
        QFile v1File(tempPath);
        v1File.open(QFile::Truncate | QFile::WriteOnly);

        // write the gpd to disk
        v1File.write(http->readAll());

        // clean up
        v1File.flush();
        v1File.close();
    }

    if (hasAwards)
    {
        hasAwards = false;
        http->get(gpdDirectory + "award/" + QString::number(entry.titleID, 16).toUpper() + ".gpd");
    }
}

void GPDDownloader::onDone(bool error)
{
    emit FinishedDownloading(gameGPD, awardGPD, entry, error);
}
