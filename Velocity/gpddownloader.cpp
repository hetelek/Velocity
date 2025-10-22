#include "gpddownloader.h"
#include <QElapsedTimer>
#include <QSettings>
#include <QDebug>
#include <QDateTime>

// Constructor initializes the GpdDownloader object, setting up the network manager for handling requests
GpdDownloader::GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject *parent)
    : QObject(parent), entry(std::move(entry)), indexIn(index), hasAwards(hasAwards) {
    // Register DownloadError type for use in queued connections
    static bool registered = false;
    if (!registered) {
        qRegisterMetaType<DownloadError>("DownloadError");
        registered = true;
    }
    
    // Connect the finished signal from the network manager to the onRequestFinished slot
    connect(&networkManager, &QNetworkAccessManager::finished, this, &GpdDownloader::onRequestFinished);
    
    // Initialize retry timer
    retryTimer = new QTimer(this);
    retryTimer->setSingleShot(true);
    connect(retryTimer, &QTimer::timeout, this, &GpdDownloader::retryDownload);
    
    // Apply timeout from settings (default 30 seconds)
    QSettings settings;
    int timeoutMs = settings.value("network/timeout", 30000).toInt();
    networkManager.setTransferTimeout(timeoutMs);
}

// Starts the download process for the game's GPD file
void GpdDownloader::BeginDownload() {
    QString msg = QString("GpdDownloader::BeginDownload() - Starting download for title %1").arg(QString::number(entry.titleID, 16).toUpper());
    qDebug() << msg;
    currentAttempt = 0;
    cancelled = false;
    downloadingAward = false;
    startDownload();
}

// Cancel the current download
void GpdDownloader::Cancel() {
    cancelled = true;
    if (currentReply) {
        currentReply->abort();
    }
    if (retryTimer && retryTimer->isActive()) {
        retryTimer->stop();
    }
}

// Internal method to start or retry a download
void GpdDownloader::startDownload() {
    if (cancelled) {
        return;
    }
    
    currentAttempt++;
    
    // Construct the URL based on whether we're downloading game or award GPD
    QString url;
    if (!downloadingAward) {
        url = QString("%1/game/%2.gpd").arg(BASE_GITHUB_URL, QString::number(entry.titleID, 16).toUpper());
    } else {
        url = QString("%1/award/%2.gpd").arg(BASE_GITHUB_URL, QString::number(entry.titleID, 16).toUpper());
    }
    
    QNetworkRequest request{QUrl(url)};
    currentReply = networkManager.get(request);
    
    // Connect progress signal
    connect(currentReply, &QNetworkReply::downloadProgress, this, &GpdDownloader::onDownloadProgress);
    
    lastBytesReceived = 0;
    lastProgressTime = QDateTime::currentMSecsSinceEpoch();
}

// Slot called when the network request is finished
void GpdDownloader::onRequestFinished(QNetworkReply *reply) {
    QString msg = QString("GpdDownloader::onRequestFinished() - Download finished for title %1 - Error: %2 - URL: %3")
        .arg(QString::number(entry.titleID, 16).toUpper())
        .arg(reply->error())
        .arg(reply->url().toString());
    qDebug() << msg;
    
    if (cancelled) {
        reply->deleteLater();
        return;
    }
    
    // Check for errors in the network reply
    if (reply->error() != QNetworkReply::NoError) {
        // Check if we should retry
        if (currentAttempt < MAX_RETRY_ATTEMPTS) {
            qDebug() << "Download attempt" << currentAttempt << "failed for title" 
                     << QString::number(entry.titleID, 16).toUpper() 
                     << "- Error:" << reply->errorString() 
                     << "- Retrying in" << (INITIAL_RETRY_DELAY_MS * (1 << (currentAttempt - 1))) << "ms";
            
            // Calculate exponential backoff delay: 1s, 2s, 4s, 8s
            int delayMs = INITIAL_RETRY_DELAY_MS * (1 << (currentAttempt - 1));
            retryTimer->start(delayMs);
            reply->deleteLater();
            return;
        }
        
        // Max retries exceeded, emit error with details
        DownloadError error = createErrorFromReply(reply, "Max retry attempts exceeded");
        emit FinishedDownloading("", "", entry, true, error);
        reply->deleteLater();
        return;
    }

    // Generate a temporary path to save the downloaded GPD file
    QString tempPath = QDir::tempPath() + "/" + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QByteArray data = reply->readAll();

    // Check if the game GPD file has already been written
    if (!downloadingAward) {
        // Save the game GPD file to the temporary path
        gameGpd = tempPath;
        QFile file(tempPath);
        
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            DownloadError error;
            error.errorMessage = "Failed to open file for writing: " + file.errorString();
            error.userFriendlyDescription = "Could not save downloaded file to disk. Check disk space and permissions.";
            emit FinishedDownloading("", "", entry, true, error);
            reply->deleteLater();
            return;
        }
        
        qint64 bytesWritten = file.write(data);
        file.close();
        
        if (bytesWritten != data.size()) {
            DownloadError error;
            error.errorMessage = QString("Incomplete write: %1 of %2 bytes").arg(bytesWritten).arg(data.size());
            error.userFriendlyDescription = "Failed to save complete file to disk. Check disk space.";
            QFile::remove(tempPath); // Clean up incomplete file
            emit FinishedDownloading("", "", entry, true, error);
            reply->deleteLater();
            return;
        }
        
        // Validate the downloaded GPD file
        QString validationError;
        if (!validateGpdFile(tempPath, validationError)) {
            DownloadError error;
            error.errorMessage = "Invalid GPD file: " + validationError;
            error.userFriendlyDescription = "Downloaded file is not a valid GPD. The file may be corrupt or missing from the repository.";
            QFile::remove(tempPath); // Clean up invalid file
            emit FinishedDownloading("", "", entry, true, error);
            reply->deleteLater();
            return;
        }
        
        gpdWritten = true;
        
        // If the game has avatar awards, initiate a second download for the award GPD file
        if (hasAwards) {
            downloadingAward = true;
            currentAttempt = 0; // Reset attempt counter for award download
            startDownload();
        } else {
            // If there are no awards, emit the FinishedDownloading signal
            QString msg = QString("GpdDownloader - Emitting success signal (game only) for %1").arg(QString::number(entry.titleID, 16).toUpper());
            qDebug() << msg;
            emit FinishedDownloading(gameGpd, "", entry, false, DownloadError());
        }
    } else {
        // Save the award GPD file to the temporary path
        awardGpd = tempPath;
        QFile file(tempPath);
        
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            DownloadError error;
            error.errorMessage = "Failed to open award file for writing: " + file.errorString();
            error.userFriendlyDescription = "Could not save downloaded award file to disk.";
            emit FinishedDownloading(gameGpd, "", entry, true, error);
            reply->deleteLater();
            return;
        }
        
        qint64 bytesWritten = file.write(data);
        file.close();
        
        if (bytesWritten != data.size()) {
            DownloadError error;
            error.errorMessage = QString("Incomplete award write: %1 of %2 bytes").arg(bytesWritten).arg(data.size());
            error.userFriendlyDescription = "Failed to save complete award file to disk.";
            QFile::remove(tempPath);
            emit FinishedDownloading(gameGpd, "", entry, true, error);
            reply->deleteLater();
            return;
        }
        
        // Validate the award GPD file
        QString validationError;
        if (!validateGpdFile(tempPath, validationError)) {
            DownloadError error;
            error.errorMessage = "Invalid award GPD file: " + validationError;
            error.userFriendlyDescription = "Downloaded award file is not valid. It may not exist in the repository.";
            QFile::remove(tempPath);
            // Don't fail completely - we have the game GPD
            emit FinishedDownloading(gameGpd, "", entry, false, DownloadError());
            reply->deleteLater();
            return;
        }
        
        // Emit the FinishedDownloading signal with both the game and award GPD paths
        qDebug() << "GpdDownloader - Emitting success signal (game + award) for" << QString::number(entry.titleID, 16).toUpper();
        emit FinishedDownloading(gameGpd, awardGpd, entry, false, DownloadError());
    }

    reply->deleteLater();
}

void GpdDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (bytesTotal <= 0) {
        return; // Can't calculate progress without total
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 elapsedMs = currentTime - lastProgressTime;
    
    if (elapsedMs > 0) {
        qint64 bytesDelta = bytesReceived - lastBytesReceived;
        double speedBytesPerSec = (bytesDelta * 1000.0) / elapsedMs;
        
        int secondsRemaining = 0;
        if (speedBytesPerSec > 0) {
            qint64 bytesRemaining = bytesTotal - bytesReceived;
            secondsRemaining = static_cast<int>(bytesRemaining / speedBytesPerSec);
        }
        
        emit ProgressUpdated(bytesReceived, bytesTotal, speedBytesPerSec, secondsRemaining);
        
        lastBytesReceived = bytesReceived;
        lastProgressTime = currentTime;
    }
}

void GpdDownloader::retryDownload() {
    startDownload();
}

DownloadError GpdDownloader::createErrorFromReply(QNetworkReply *reply, const QString &additionalInfo) {
    DownloadError error;
    
    if (reply) {
        error.networkError = reply->error();
        error.errorMessage = reply->errorString();
        
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (statusCode.isValid()) {
            error.httpStatusCode = statusCode.toInt();
        }
        
        // Create user-friendly descriptions
        switch (error.networkError) {
            case QNetworkReply::ConnectionRefusedError:
                error.userFriendlyDescription = "Connection refused. Check your internet connection.";
                break;
            case QNetworkReply::RemoteHostClosedError:
                error.userFriendlyDescription = "Remote server closed the connection. Please try again.";
                break;
            case QNetworkReply::HostNotFoundError:
                error.userFriendlyDescription = "Server not found. Check your internet connection.";
                break;
            case QNetworkReply::TimeoutError:
                error.userFriendlyDescription = "Download timed out. The server may be slow or unreachable.";
                break;
            case QNetworkReply::OperationCanceledError:
                error.userFriendlyDescription = "Download was cancelled.";
                break;
            case QNetworkReply::SslHandshakeFailedError:
                error.userFriendlyDescription = "SSL connection failed. Check your network security settings.";
                break;
            case QNetworkReply::TemporaryNetworkFailureError:
                error.userFriendlyDescription = "Temporary network failure. Please try again.";
                break;
            case QNetworkReply::NetworkSessionFailedError:
                error.userFriendlyDescription = "Network session failed. Check your internet connection.";
                break;
            case QNetworkReply::BackgroundRequestNotAllowedError:
                error.userFriendlyDescription = "Background downloads are not allowed by your system.";
                break;
            case QNetworkReply::TooManyRedirectsError:
                error.userFriendlyDescription = "Too many redirects. The server configuration may be incorrect.";
                break;
            case QNetworkReply::InsecureRedirectError:
                error.userFriendlyDescription = "Insecure redirect detected. The connection is not secure.";
                break;
            case QNetworkReply::ProxyConnectionRefusedError:
            case QNetworkReply::ProxyConnectionClosedError:
            case QNetworkReply::ProxyNotFoundError:
            case QNetworkReply::ProxyTimeoutError:
            case QNetworkReply::ProxyAuthenticationRequiredError:
                error.userFriendlyDescription = "Proxy error. Check your proxy settings.";
                break;
            case QNetworkReply::ContentAccessDenied:
            case QNetworkReply::ContentOperationNotPermittedError:
            case QNetworkReply::AuthenticationRequiredError:
                error.userFriendlyDescription = "Access denied. You may not have permission to download this file.";
                break;
            case QNetworkReply::ContentNotFoundError:
                error.userFriendlyDescription = "File not found. This game's GPD may not be available in the repository.";
                break;
            case QNetworkReply::ProtocolUnknownError:
            case QNetworkReply::ProtocolInvalidOperationError:
            case QNetworkReply::ProtocolFailure:
                error.userFriendlyDescription = "Protocol error. The download request failed.";
                break;
            case QNetworkReply::InternalServerError:
            case QNetworkReply::ServiceUnavailableError:
                error.userFriendlyDescription = "Server error. The remote server is experiencing issues.";
                break;
            default:
                if (error.httpStatusCode >= 400 && error.httpStatusCode < 500) {
                    error.userFriendlyDescription = QString("Client error (HTTP %1). The request was invalid.").arg(error.httpStatusCode);
                } else if (error.httpStatusCode >= 500) {
                    error.userFriendlyDescription = QString("Server error (HTTP %1). The server is having issues.").arg(error.httpStatusCode);
                } else {
                    error.userFriendlyDescription = "An unknown error occurred during download.";
                }
                break;
        }
    }
    
    if (!additionalInfo.isEmpty()) {
        error.errorMessage += " - " + additionalInfo;
    }
    
    return error;
}

bool GpdDownloader::validateGpdFile(const QString &filePath, QString &errorMsg) {
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        errorMsg = "Cannot open file for validation";
        return false;
    }
    
    // Check minimum size (XDBF header is at least 0x28 bytes)
    if (file.size() < 0x28) {
        errorMsg = QString("File too small (%1 bytes, minimum 40 bytes)").arg(file.size());
        return false;
    }
    
    // Read and check magic number
    QByteArray header = file.read(4);
    if (header.size() != 4) {
        errorMsg = "Cannot read file header";
        return false;
    }
    
    // XDBF magic number is 0x58444246 ("XDBF" in ASCII)
    quint32 magic = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(header.constData()));
    if (magic != 0x58444246) {
        errorMsg = QString("Invalid magic number: 0x%1 (expected 0x58444246)").arg(magic, 8, 16, QChar('0'));
        return false;
    }
    
    file.close();
    return true;
}


