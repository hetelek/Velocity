#ifndef AVATARASSETDOWNLOADER_H
#define AVATARASSETDOWNLOADER_H

// qt
#include <QObject>
#include <QUuid>
#include <QUrl>
#include <QDir>
#include <QDebug>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

// other
#include <XboxInternals/TypeDefinitions.h>
#include <XboxInternals/IO/FileIO.h>

class AvatarAssetDownloader : public QObject
{
    Q_OBJECT
public:
    explicit AvatarAssetDownloader(QString titleID, QString guid, QObject *parent = 0);
    QString GetV1TempPath();
    QString GetV2TempPath();
    QString GetGUID();
    QString GetTitleID();
    void BeginDownload();

signals:
    void FinishedDownloading();

public slots:
    void onDone(bool);
    void onRequestFinished(QNetworkReply *aReply);

private:
    QNetworkAccessManager *manager;

    QString titleID;
    QString guid;

    QString v1TempPath;
    QString v2TempPath;

    int idToSkip;

    bool v1Done;
    bool v2Done;
};

#endif // AVATARASSETDOWNLOADER_H


