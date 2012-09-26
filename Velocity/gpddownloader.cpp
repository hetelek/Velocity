#include "gpddownloader.h"

GPDDownloader::GPDDownloader(QString host, QString gpdDirectory, QObject *parent) : gpdDirectory(gpdDirectory)
{
    gpdDirectory = gpdDirectory.trimmed();

    http = new QHttp(this);
    http->setHost(host);

    connect(http, SIGNAL(done(bool)), this, SLOT(onDone(bool)));
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(onRequestFinished(int, bool)));
}

void GPDDownloader::BeginDownload(QString titleId, bool hasAvatarAwards)
{
    // setup
    titleID = titleId;
    gpdFinished = false;
    awardGPDFinished = !hasAvatarAwards;

    // clear the current paths
    awardGPD.clear();
    gameGPD.clear();

    // begin the download
    http->get(gpdDirectory + titleID.toUpper() + ".gpd");
}

QString GPDDownloader::GetTempAwardGPDPath()
{
    return awardGPD;
}

QString GPDDownloader::GetTempGameGPDPath()
{
    return gameGPD;
}

void GPDDownloader::onRequestFinished(int id, bool error)
{
    if (error)
        return;

    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");

    if (!gpdFinished)
    {
        gpdFinished = true;
        gameGPD = tempPath;

        // create a new temporary file
        QFile v1File(tempPath);
        v1File.open(QFile::Truncate | QFile::WriteOnly);

        // write the gpd to disk
        v1File.write(http->readAll());

        // clean up
        v1File.flush();
        v1File.close();
    }
    else if (awardGPDFinished)
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

    if (!awardGPDFinished)
    {
        awardGPDFinished = true;
        http->get(gpdDirectory + "awards/" + titleID + ".gpd");
        return;
    }
}

void GPDDownloader::onDone(bool error)
{
    if (!error)
    {
        emit FinishedDownloading(gameGPD, awardGPD, titleID);
    }
}
