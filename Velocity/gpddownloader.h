#ifndef GPDDOWNLOADER_H
#define GPDDOWNLOADER_H

// qt
#include <QObject>
#include <QHttp>
#include <QUuid>
#include <QUrl>
#include <QDir>
#include <QDebug>

// other
#include "winnames.h"
#include "GPD/DashboardGPD.h"
#include "FileIO.h"

class GPDDownloader : public QObject
{
    Q_OBJECT
public:
    explicit GPDDownloader(TitleEntry entry, int index, bool hasAwards = false, QObject *parent = 0);
    void BeginDownload();
    int index();

signals:
    void FinishedDownloading(QString, QString, TitleEntry, bool);

public slots:
    void onDone(bool);
    void onRequestFinished(int, bool);

private:
    QHttp *http;
    QString gpdDirectory, titleID, awardGPD, gameGPD;
    TitleEntry entry;
    bool hasAwards, gpdWritten;
    int indexIn;
};

#endif // GPDDOWNLOADER_H
