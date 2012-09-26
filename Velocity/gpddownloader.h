#ifndef GPDDOWNLOADER_H
#define GPDDOWNLOADER_H

// qt
#include <QObject>
#include <QHttp>
#include <QUuid>
#include <QUrl>
#include <QDir>

// other
#include "winnames.h"
#include "FileIO.h"

class GPDDownloader : public QObject
{
    Q_OBJECT
public:
    explicit GPDDownloader(QString host, QString gpdDirectory, QObject *parent = 0);
    void BeginDownload(QString titleId, bool hasAvatarAwards = false);
    QString GetTempAwardGPDPath();
    QString GetTempGameGPDPath();

signals:
    void FinishedDownloading(QString gameGPDPath, QString awardGPDPath, QString titleID);

public slots:
    void onDone(bool);
    void onRequestFinished(int, bool);

private:
    QHttp *http;
    QString gpdDirectory, titleID, awardGPD, gameGPD;

    bool gpdFinished, awardGPDFinished;
};

#endif // GPDDOWNLOADER_H
