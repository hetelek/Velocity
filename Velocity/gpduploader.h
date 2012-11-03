#ifndef GPDUPLOADER_H
#define GPDUPLOADER_H

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

#include "GPD/GameGPD.h"
#include "GPD/AvatarAwardGPD.h"

struct GPDPaths
{
    QString gameGPD, awardGPD;
};

class GPDUploader : public QObject
{
    Q_OBJECT

public:
    explicit GPDUploader(QStringList gamePaths, QStringList avatarPaths, QStringList titleIDs, bool deleteGPDs, QObject *parent);

signals:
    void FinishedUploading(int success, int failures);

private slots:
    void reply(QNetworkReply *reply);

private:
    QSettings *settings;

    QStringList gamePaths, avatarPaths, titleIDs;
    int success, failures, currentIndex;
    bool deleteGPDs;

    QNetworkAccessManager *networkAccessManager;
    void sendRequest(QString filePath, QString awardFilePath, QString gameName, QString titleID, DWORD achievementCount, DWORD gamerscoreTotal, DWORD awards, DWORD mAwards, DWORD fAwards);
    void uploadGPD(QString gamePath, QString awardPath, QString titleID);
};

#endif // GPDUPLOADER_H
