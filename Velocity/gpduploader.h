#ifndef GPDUPLOADER_H
#define GPDUPLOADER_H

#include <QObject>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDebug>

// network includes
    #include <QByteArray>
    #include <QNetworkAccessManager>
    #include <QNetworkRequest>
    #include <QNetworkReply>
//

#include "GPD/GameGPD.h"
#include "GPD/AvatarAwardGPD.h"

class GPDUploader : public QObject
{
    Q_OBJECT

public:
    explicit GPDUploader(QString path, QString avatarPath, QObject *parent = 0);
    void UploadGPD();

private slots:
    void reply(QNetworkReply *reply);

private:
    QString path, avatarPath;
    int count;

    QNetworkAccessManager *networkAccessManager;
    void sendRequest(QString filePath, QString awardFilePath, QString gameName, QString titleID, DWORD achievementCount, DWORD gamerscoreTotal, DWORD awards, DWORD mAwards, DWORD fAwards);
};

#endif // GPDUPLOADER_H
