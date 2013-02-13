#include "gpduploader.h"

GpdUploader::GpdUploader(QStringList gamePaths, QStringList avatarPaths, QStringList titleIDs, bool deleteGpds, QObject *parent = 0) : QObject(parent), gamePaths(gamePaths), avatarPaths(avatarPaths), titleIDs(titleIDs), deleteGpds(deleteGpds)
{
    settings = new QSettings("Exetelek", "Velocity");

    // only if they selected to share data with Velocity
    if (settings->value("AnonData").toBool())
    {
        // initalize variables
        success = 0;
        failures = 0;
        currentIndex = 0;

        networkAccessManager = new QNetworkAccessManager(this);
        connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply(QNetworkReply*)));

        // begin the first upload
        if (gamePaths.count() > 0)
            uploadGpd(gamePaths.at(currentIndex), avatarPaths.at(currentIndex), titleIDs.at(currentIndex));
    }
    else
    {
        // remove temporary files
        foreach (QString gamePath, gamePaths)
            QFile::remove(gamePath);

        foreach (QString avatarPath, avatarPaths)
            QFile::remove(avatarPath);
    }
}

void GpdUploader::uploadGpd(QString gamePath, QString awardPath, QString titleID)
{
    GameGpd *gpd = NULL;
    try
    {
        // open the Gpd
        try
        {
            gpd = new GameGpd(gamePath.toStdString());
        }
        catch (string error)
        {
            if (gpd)
                delete gpd;

            qDebug() << titleID << " Error: " << QString::fromStdString(error);
            return;
        }

        gpd->StartWriting();

        // calculate total gamerscore
        int totalGamerscore = 0;
        for (DWORD x = 0; x < gpd->achievements.size(); x++)
            totalGamerscore += gpd->achievements.at(x).gamerscore;

        // check for award Gpd
        DWORD awards = 0, mAwards = 0, fAwards = 0;
        if (QFile::exists(awardPath))
        {
            // open the award gpd, get the amount of awards
            AvatarAwardGpd agpd(awardPath.toStdString());
            awards = agpd.avatarAwards.size();

            for (DWORD x = 0; x < agpd.avatarAwards.size(); x++)
            {
                struct AvatarAward award = agpd.avatarAwards.at(x);
                AssetGender g = AvatarAwardGpd::GetAssetGender(&award);

                // lock all awards
                award.flags &= 0xF;
                agpd.WriteAvatarAward(&award);

                // caluclate male/female award counts
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

            // close award file
            agpd.Close();
        }


        // remove all setting entries
        int iterateCount = gpd->settings.size();
        for (int x = 0; x < iterateCount; x++)
            gpd->DeleteSettingEntry(gpd->settings.at(0));

        // remove all images
        iterateCount = gpd->images.size();
        char hitTitleImage = 0;
        for (int x = 0; x < iterateCount; x++)
        {
            if (gpd->images.at(hitTitleImage).entry.id != TitleInformation)
                gpd->DeleteImageEntry(gpd->images.at(hitTitleImage));
            else
                hitTitleImage++;
        }

        // lock all achievements
        for (DWORD x = 0; x < gpd->achievements.size(); x++)
        {
            AchievementEntry chiev = gpd->achievements.at(x);
            chiev.flags &= 0xF;
            gpd->WriteAchievementEntry(&chiev);
        }


        qDebug() << "game name: " << QString::fromStdWString(gpd->gameName.ws);

        // clean the gpd
        gpd->StopWriting();
        gpd->CleanGpd();
        gpd->Close();

        // get the arguments before deleting the gpd
        QString titleName = QString::fromStdWString(gpd->gameName.ws);
        DWORD achievementCount = gpd->achievements.size();
        delete gpd;
        gpd = NULL;

        // send the Gpd(s) to the server
        sendRequest(gamePath, (QFile::exists(awardPath)) ? awardPath : "", titleName, titleID, achievementCount, totalGamerscore, awards, mAwards, fAwards);
    }
    catch (string s)
    {
        qDebug() << QString::fromStdString(s) + "\r\n" + gamePath;
    }
    catch(...)
    {
        qDebug() << "unknown error (uploading gpd)";
    }

    if (gpd)
        delete gpd;
}

void GpdUploader::reply(QNetworkReply *reply)
{
    // print out the reply/error message
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

    // begin the next request, if there is one
    if (gamePaths.count() > ++currentIndex)
        uploadGpd(gamePaths.at(currentIndex), avatarPaths.at(currentIndex), titleIDs.at(currentIndex));
}

void GpdUploader::sendRequest(QString filePath, QString awardFilePath, QString gameName, QString titleID, DWORD achievementCount, DWORD gamerscoreTotal, DWORD awards, DWORD mAwards, DWORD fAwards)
{
    try
    {
        // setup the GET parameters
        QString getData = "?nm=" + gameName + "&tid=" + titleID.toUpper() + "&achc=" + QString::number(achievementCount) + "&ttlgs=" +
                    QString::number(gamerscoreTotal) + "&ttlac=" + QString::number(awards) + "&ttlmac=" + QString::number(mAwards) + "&ttlfac=" + QString::number(fAwards);

        // open the Gpd file
        QFile gpdFile(filePath);
        if (!gpdFile.open(QIODevice::ReadWrite))
            return;

        // open the award file (if there is one)
        QFile awardFile(awardFilePath);
        if (awardFilePath != "")
            awardFile.open(QIODevice::ReadWrite);

        // initialize boundrary
        QString boundary = "-----------------------------7d935033608e2";

        // byte array to be sent in POST
        QByteArray dataToSend;

        // add data to the variable to be sent
        dataToSend.append("\r\n--" + boundary + "\r\n");
        dataToSend.append("Content-Disposition: form-data; name=\"game\"; filename=\"" + titleID.toUpper() + ".gpd\"\r\n");
        dataToSend.append("Content-Type: application/octet-stream\r\n\r\n");
        dataToSend.append(gpdFile.readAll());

        // remove/close the gpd
        gpdFile.close();
        if (deleteGpds)
            if (!QFile::remove(filePath))
                qDebug() << "failed to remove gpd file";

        // setup next boundary
        dataToSend.append("\r\n--" + boundary + "\r\n");

        // add the award gpd to the data to be sent, if there is an award gpd
        if (awardFilePath != "")
        {
            dataToSend.append("Content-Disposition: form-data; name=\"award\"; filename=\"" + titleID.toUpper() + ".gpd\"\r\n");
            dataToSend.append("Content-Type: application/octet-stream\r\n\r\n");
            dataToSend.append(awardFile.readAll());

            // remove/close the award gpd
            if (deleteGpds) {
                if (!awardFile.remove())
                    qDebug() << "failed to remove award file";
            } else
                awardFile.close();
        }

        // end the data to be sent
        dataToSend.append("\r\n--" + boundary + "--\r\n");

        // set the network request
        QNetworkRequest request(QUrl("http://velocity.expetelek.com/gameadder/add.php" + getData));
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
