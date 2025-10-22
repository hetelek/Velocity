#include "AvatarAssetDownloader.h"

AvatarAssetDownloader::AvatarAssetDownloader(QString titleID, QString guid, QObject *parent) :
    QObject(parent), titleID(titleID), guid(guid), v1TempPath(QString("")), v2TempPath(QString("")),
    v1Done(false), v2Done(false)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));

    /*http = new QHttp(this);
    http->setHost("download.xboxlive.com");

    connect(http, SIGNAL(done(bool)), this, SLOT(onDone(bool)));
    connect(http, SIGNAL(requestFinished(int,bool)), this, SLOT(onRequestFinished(int,bool)));*/
}

void AvatarAssetDownloader::BeginDownload()
{
    manager->get(QNetworkRequest(QUrl("http://download.xboxlive.com/content/" + titleID +
            "/avataritems/" + guid + ".bin")));
    //http->get("http://download.xboxlive.com/content/" + titleID + "/avataritems/" + guid + ".bin");
    //idToSkip = http->currentId();
}

QString AvatarAssetDownloader::GetV1TempPath()
{
    return v1TempPath;
}

QString AvatarAssetDownloader::GetV2TempPath()
{
    return v2TempPath;
}

QString AvatarAssetDownloader::GetGUID()
{
    return guid;
}

QString AvatarAssetDownloader::GetTitleID()
{
    return titleID;
}

void AvatarAssetDownloader::onRequestFinished(QNetworkReply *aReply)
{
    if (aReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "error: " << aReply->errorString();
        return;
    }

    // verify that the file was downloaded
    DWORD fileSize = aReply->bytesAvailable();

    // all assets have a Ytgr header that's 0x140 bytes
    if (fileSize < 0x140)
    {
        if (!v2Done)
        {
            v1Done = true;
            v2Done = true;
            v1TempPath = "";
            manager->get(QNetworkRequest(QUrl("http://download.xboxlive.com/content/" + titleID +
                    "/avataritems/v2/" + guid + ".bin")));
        }
        else
            v2TempPath = "";
        return;
    }

    // read the crap away, we don't need it
    BYTE temp[0x140];
    aReply->read((char*)temp, 0x140);

    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{",
            "").replace("}", "").replace("-", "");
    if (!v1Done)
        v1TempPath = tempPath;
    else
        v2TempPath = tempPath;

    // create a new temporary file
    QFile v1File(tempPath);
    v1File.open(QFile::Truncate | QFile::WriteOnly);
    // Write the STRB file to the local disk
    v1File.write(aReply->readAll());

    // clean up
    v1File.flush();
    v1File.close();

    v1Done = true;

    // download the v2 file
    if (!v2Done)
    {
        v2Done = true;
        manager->get(QNetworkRequest(QUrl("http://download.xboxlive.com/content/" + titleID +
                "/avataritems/v2/" + guid + ".bin")));
    }
    else
        emit FinishedDownloading();
}

void AvatarAssetDownloader::onDone(bool /* error */)
{
    //if (!error)
    //emit FinishedDownloading();
}


