#include "gpddownloader.h"

GpdDownloader::GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject * /* parent */ ) : entry(entry), indexIn(index), hasAwards(hasAwards)
{
    gpdDirectory = "/gameadder/";
    gpdWritten = false;

    http = new QHttp(this);
    http->setHost("velocity.expetelek.com");

    connect(http, SIGNAL(done(bool)), this, SLOT(onDone(bool)));
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(onRequestFinished(int, bool)));
}

void GpdDownloader::BeginDownload()
{
    QString url = gpdDirectory + "game/" + QString::number(entry.titleID, 16).toUpper() + ".gpd";
    http->get(url);
}

int GpdDownloader::index()
{
    return indexIn;
}

void GpdDownloader::onRequestFinished(int /* id */, bool error)
{
    if (error)
        qDebug() << http->errorString();
    else if (http->bytesAvailable() < 1)
        return;

    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

    if (!gpdWritten)
    {
        gameGpd = tempPath;

        // create a new temporary file
        QFile v1File(tempPath);
        v1File.open(QFile::Truncate | QFile::WriteOnly);

        // Write the gpd to disk
        v1File.write(http->readAll());

        // clean up
        v1File.flush();
        v1File.close();

        gpdWritten = true;
    }
    else
    {
        awardGpd = tempPath;

        // create a new temporary file
        QFile v1File(tempPath);
        v1File.open(QFile::Truncate | QFile::WriteOnly);

        // Write the gpd to disk
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

void GpdDownloader::onDone(bool error)
{
    emit FinishedDownloading(gameGpd, awardGpd, entry, error);
}
