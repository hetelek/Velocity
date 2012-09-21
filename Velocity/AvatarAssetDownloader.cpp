#include "AvatarAssetDownloader.h"
#include <QMessageBox>

AvatarAssetDownloader::AvatarAssetDownloader(QString titleID, QString guid, QObject *parent) :
    QObject(parent), titleID(titleID), guid(guid), v1Done(false), v2Done(false)
{
    http = new QHttp(this);
    http->setHost("download.xboxlive.com");

    connect(http, SIGNAL(done(bool)), this, SLOT(onDone(bool)));
    connect(http, SIGNAL(requestFinished(int,bool)), this, SLOT(onRequestFinished(int,bool)));

    http->get("http://download.xboxlive.com/content/" + titleID + "/avataritems/" + guid + ".bin");
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

void AvatarAssetDownloader::onRequestFinished(int id, bool error)
{
    if (error)
        return;

    // verify that the file was downloaded
    DWORD fileSize = http->bytesAvailable();

    // all assets have a YTGR header that's 0x140 bytes
    if (fileSize < 0x140)
    {
        v2Done = true;
        http->get("http://download.xboxlive.com/content/" + titleID + "/avataritems/v2/" + guid + ".bin");
        return;
    }

    // read the crap away, we don't need it
    BYTE temp[0x140];
    http->read(temp, 0x140);

    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString().replace("{", "").replace("}", "").replace("-", "");
    if (!v1Done)
        v1TempPath = tempPath;
    else
        v2TempPath = tempPath;

    // create a new temporary file
    QFile v1File(tempPath);
    v1File.open(QFile::Truncate | QFile::WriteOnly);
    // write the STRB file to the local disk
    v1File.write(http->readAll());

    // clean up
    v1File.flush();
    v1File.close();

    v1Done = true;

    // download the v2 file
    if (!v2Done)
    {
        v2Done = true;
        http->get("http://download.xboxlive.com/content/" + titleID + "/avataritems/v2/" + guid + ".bin");
    }
}

void AvatarAssetDownloader::onDone(bool error)
{
    if (!error)
        emit FinishedDownloading();
}
