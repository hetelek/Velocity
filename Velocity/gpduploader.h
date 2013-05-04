#ifndef GpdUPLOADER_H
#define GpdUPLOADER_H

#include <QObject>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>

// network includes
    #include <QByteArray>
    #include <QNetworkAccessManager>
    #include <QNetworkRequest>
    #include <QNetworkReply>
//

#include "Gpd/GameGpd.h"
#include "Gpd/AvatarAwardGpd.h"

struct GpdPaths
{
    QString gameGpd, awardGpd;
};

class GpdUploader : public QObject
{
    Q_OBJECT

public:
    explicit GpdUploader(QStringList gamePaths, QStringList avatarPaths, QStringList titleIDs, bool deleteGpds, QObject *parent);

signals:
    void FinishedUploading(int success, int failures);

private slots:
    void reply(QNetworkReply *reply);

private:
    QSettings *settings;

    QStringList gamePaths, avatarPaths, titleIDs;
    int success, failures, currentIndex;
    bool deleteGpds;

    QNetworkAccessManager *networkAccessManager;
    void sendRequest(QString filePath, QString awardFilePath, QString gameName, QString titleID, DWORD achievementCount, DWORD gamerscoreTotal, DWORD awards, DWORD mAwards, DWORD fAwards);
    void uploadGpd(QString gamePath, QString awardPath, QString titleID);
};

#endif // GpdUPLOADER_H
