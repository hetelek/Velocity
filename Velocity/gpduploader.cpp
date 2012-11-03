#include "gpduploader.h"

GPDUploader::GPDUploader(QStringList gamePaths, QStringList avatarPaths, QStringList titleIDs, bool deleteGPDs, QObject *parent = 0) : QObject(parent), deleteGPDs(deleteGPDs), gamePaths(gamePaths), titleIDs(titleIDs), avatarPaths(avatarPaths)
{
    settings = new QSettings("Exetelek", "Velocity");

    if (settings->value("AnonData").toBool())
    {
        success = 0;
        failures = 0;
        currentIndex = 0;

        networkAccessManager = new QNetworkAccessManager(this);
        connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));

        if (gamePaths.count() > 0)
            uploadGPD(gamePaths.at(currentIndex), avatarPaths.at(currentIndex), titleIDs.at(currentIndex));
    }
}

void GPDUploader::uploadGPD(QString gamePath, QString awardPath, QString titleID)
{
    try
    {
        GameGPD *gpd;
        try
        {
            gpd = new GameGPD(gamePath.toStdString());
        }
        catch (string error)
        {
            qDebug() << QString::fromStdString(error);
            return;
        }


        if (gpd->achievements.size() == 0)
        {
            gpd->Close();
            delete gpd;
            return;
        }

        int totalGamerscore = 0;
        for (DWORD x = 0; x < gpd->achievements.size(); x++)
            totalGamerscore += gpd->achievements.at(x).gamerscore;

        DWORD awards = 0, mAwards = 0, fAwards = 0;
        if (QFile::exists(awardPath))
        {
            AvatarAwardGPD agpd(awardPath.toStdString());
            awards = agpd.avatarAwards.size();

            for (DWORD x = 0; x < agpd.avatarAwards.size(); x++)
            {
                struct AvatarAward award = agpd.avatarAwards.at(x);
                AssetGender g = AvatarAwardGPD::GetAssetGender(&award);
                award.flags &= 0xF;
                agpd.WriteAvatarAward(&award);

                if (g == Male)
                    mAwards++;
                else if (g == Female)
                    fAwards++;
                else
                {
                    mAwards++;
                    fAwards++;
                }
            }

            agpd.Close();
        }


        int iterateCount = gpd->settings.size();
        for (DWORD x = 0; x < iterateCount; x++)
            gpd->DeleteSettingEntry(gpd->settings.at(0));

        iterateCount = gpd->images.size();
        char hitTitleImage = 0;
        for (DWORD x = 0; x < iterateCount; x++)
        {
            if (gpd->images.at(hitTitleImage).entry.id != TitleInformation)
                gpd->DeleteImageEntry(gpd->images.at(hitTitleImage));
            else
                hitTitleImage++;
        }

        for (DWORD x = 0; x < gpd->achievements.size(); x++)
        {
            AchievementEntry chiev = gpd->achievements.at(x);
            chiev.flags &= 0xF;
            gpd->WriteAchievementEntry(&chiev);
        }

        gpd->CleanGPD();

        qDebug() << QString::fromStdWString(gpd->gameName.ws);
        sendRequest(gamePath, (QFile::exists(awardPath)) ? awardPath : "",
                    QString::fromStdWString(gpd->gameName.ws), titleID, gpd->achievements.size(), totalGamerscore, awards, mAwards, fAwards);

        gpd->Close();
        delete gpd;
    }
    catch (string s)
    {
        qDebug() << QString::fromStdString(s) + "\r\n" + gamePath;
        return;
    }
    catch(...)
    {
        qDebug() << "unknown error (uploading gpd)";
    }
}

void GPDUploader::reply(QNetworkReply *reply)
{
    if (reply->error() || reply->bytesAvailable() < 1)
    {
        failures++;
        qDebug() << "error occured: " << reply->errorString();
    }
    else
    {
        success++;
        qDebug() << "response recieved: " + reply->readAll();
    }

    if (gamePaths.count() > ++currentIndex)
        uploadGPD(gamePaths.at(currentIndex), avatarPaths.at(currentIndex), titleIDs.at(currentIndex));
}

void GPDUploader::sendRequest(QString filePath, QString awardFilePath, QString gameName, QString titleID, DWORD achievementCount, DWORD gamerscoreTotal, DWORD awards, DWORD mAwards, DWORD fAwards)
{
    try
    {
        QString getData = "?nm=" + gameName + "&tid=" + titleID.toUpper() + "&achc=" + QString::number(achievementCount) + "&ttlgs=" +
                    QString::number(gamerscoreTotal) + "&ttlac=" + QString::number(awards) + "&ttlmac=" + QString::number(mAwards) + "&ttlfac=" + QString::number(fAwards);

        QFile gpdFile(filePath);
        if (!gpdFile.open(QIODevice::ReadWrite))
            return;

        QFile awardFile(awardFilePath);
        if (awardFilePath != "")
            awardFile.open(QIODevice::ReadWrite);

        QString boundary = "-----------------------------7d935033608e2";

        QByteArray dataToSend; // byte array to be sent in POST
        dataToSend.append("\r\n--" + boundary + "\r\n");
        dataToSend.append("Content-Disposition: form-data; name=\"game\"; filename=\"" + titleID.toUpper() + ".gpd\"\r\n");
        dataToSend.append("Content-Type: application/octet-stream\r\n\r\n");
        dataToSend.append(gpdFile.readAll());

        if (deleteGPDs)
            gpdFile.remove();
        else
            gpdFile.close();

        dataToSend.append("\r\n--" + boundary + "\r\n");

        if (awardFilePath != "")
        {
            dataToSend.append("Content-Disposition: form-data; name=\"award\"; filename=\"" + titleID.toUpper() + ".gpd\"\r\n");
            dataToSend.append("Content-Type: application/octet-stream\r\n\r\n");
            dataToSend.append(awardFile.readAll());

            if (deleteGPDs)
                awardFile.remove();
            else
                awardFile.close();
        }

        dataToSend.append("\r\n--" + boundary + "--\r\n");

        QNetworkRequest request(QUrl("http://velocity.expetelek.com/gpds/index.php" + getData));
        request.setRawHeader("Content-Type","multipart/form-data; boundary=-----------------------------7d935033608e2");
        request.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend.size());

        // perform POST request
        networkAccessManager->post(request, dataToSend);
    }
    catch (...)
    {
        qDebug() << "unknown error (uploading gpd) at sendRequest";
    }
}
