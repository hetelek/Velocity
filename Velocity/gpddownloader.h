#ifndef GpdDOWNLOADER_H
#define GpdDOWNLOADER_H

// qt
#include <QObject>
#include <QHttp>
#include <QUuid>
#include <QUrl>
#include <QDir>
#include <QDebug>

// other
#include "winnames.h"
#include "Gpd/DashboardGpd.h"
#include "IO/FileIO.h"

class GpdDownloader : public QObject
{
    Q_OBJECT
public:
    explicit GpdDownloader(TitleEntry entry, int index, bool hasAwards = false, QObject *parent = 0);
    void BeginDownload();
    int index();

signals:
    void FinishedDownloading(QString, QString, TitleEntry, bool);

public slots:
    void onDone(bool);
    void onRequestFinished(int, bool);

private:
    QHttp *http;
    QString gpdDirectory, titleID, awardGpd, gameGpd;
    TitleEntry entry;
    bool hasAwards, gpdWritten;
    int indexIn;
};

#endif // GpdDOWNLOADER_H
