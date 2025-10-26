#ifndef GPDDOWNLOADER_H
#define GPDDOWNLOADER_H

#include <XboxInternals/Gpd/XdbfDefinitions.h>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QString>
#include <QTimer>

// Structure to hold detailed download error information
struct DownloadError {
    int httpStatusCode = 0;
    QNetworkReply::NetworkError networkError = QNetworkReply::NoError;
    QString errorMessage;
    QString userFriendlyDescription;
    
    bool hasError() const { return networkError != QNetworkReply::NoError || httpStatusCode >= 400; }
};

// Register DownloadError with Qt's meta-object system for use in signals/slots
Q_DECLARE_METATYPE(DownloadError)

class GpdDownloader : public QObject {
    Q_OBJECT

public:
    // Constructor
    explicit GpdDownloader(TitleEntry entry, int index, bool hasAwards, QObject *parent = nullptr);

    // Start the download process
    void BeginDownload();

    // Cancel the current download
    void Cancel();

    // Get the index of the download
    [[nodiscard]] int index() const noexcept { return indexIn; }

signals:
    void FinishedDownloading(QString gamePath, QString awardPath, TitleEntry entry, bool error, DownloadError errorDetails = DownloadError());
    void ProgressUpdated(qint64 bytesReceived, qint64 bytesTotal, double speedBytesPerSec, int secondsRemaining);

private slots:
    // Handle the network request completion
    void onRequestFinished(QNetworkReply *reply);
    
    // Handle download progress updates
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    
    // Retry the download after a delay
    void retryDownload();

private:
    // Helper to start or retry a download
    void startDownload();
    
    // Helper to create error details from reply
    DownloadError createErrorFromReply(QNetworkReply *reply, const QString &additionalInfo = QString());
    
    // Validate downloaded GPD file
    bool validateGpdFile(const QString &filePath, QString &errorMsg);

    // Static base URL for GPD files
    static inline const QString BASE_GITHUB_URL = "https://raw.githubusercontent.com/Pandoriaantje/xbox360-gpd-files/main";
    
    // Retry configuration
    static constexpr int MAX_RETRY_ATTEMPTS = 5;
    static constexpr int INITIAL_RETRY_DELAY_MS = 1000; // 1 second

    TitleEntry entry;
    int indexIn;
    bool hasAwards;
    bool gpdWritten = false;
    bool cancelled = false;
    
    int currentAttempt = 0;
    bool downloadingAward = false;

    QString gameGpd;
    QString awardGpd;
    QString gpdDirectory = "/gameadder/";

    QNetworkAccessManager networkManager;
    QNetworkReply *currentReply = nullptr;
    QTimer *retryTimer = nullptr;
    
    // Progress tracking
    qint64 lastBytesReceived = 0;
    qint64 lastProgressTime = 0;
};

#endif // GPDDOWNLOADER_H


