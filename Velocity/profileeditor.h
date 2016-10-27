#ifndef PROFILEEDITOR_H
#define PROFILEEDITOR_H

// qt
#include <QDialog>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QDir>
#include <QUuid>
#include <QFileDialog>
#include <QBuffer>
#include <QLabel>
#include <QMenu>
#include <QStatusBar>
#include <QMap>
#include "qthelpers.h"

// xbox360
#include "Stfs/StfsPackage.h"
#include "Gpd/DashboardGpd.h"
#include "Gpd/GameGpd.h"
#include "Gpd/AvatarAwardGpd.h"
#include "AvatarAsset/AvatarAssetDefinintions.h"
#include "Account/Account.h"
#include "Account/AccountHelpers.h"

// other
#include <iostream>
#include <vector>
#include <stdio.h>
#include "gpduploader.h"
#include "achievementcreationwizard.h"
#include "AvatarAssetDownloader.h"

using namespace std;

namespace Ui {
class ProfileEditor;
}

struct RegionStr
{
    QString name;
    DWORD value;
};

const RegionStr regions[] =
{
    { "Albania", 2 },
    { "Algeria", 0x1b },
    { "Argentina", 4 },
    { "Armenia", 3 },
    { "Australia", 6 },
    { "Austria", 5 },
    { "Azerbaijan", 7 },
    { "Bahrain", 10 },
    { "Belarus", 14 },
    { "Belgium", 8 },
    { "Belize", 15 },
    { "Bolivia", 12 },
    { "Brazil", 13 },
    { "Brunei Darussalam", 11 },
    { "Bulgaria", 9 },
    { "Canada", 0x10 },
    { "Chile", 0x13 },
    { "China", 20 },
    { "Colombia", 0x15 },
    { "Costa Rica", 0x16 },
    { "Croatia", 0x29 },
    { "Czech Republic", 0x17 },
    { "Denmark", 0x19 },
    { "Dominican Republic", 0x1a },
    { "Ecuador", 0x1c },
    { "Egypt", 30 },
    { "El Salvador", 0x5f },
    { "Estonia", 0x1d },
    { "Faroe Islands", 0x21 },
    { "Finland", 0x20 },
    { "France", 0x22 },
    { "Georgia", 0x24 },
    { "Germany", 0x18 },
    { "Greece", 0x25 },
    { "Guatemala", 0x26 },
    { "Honduras", 40 },
    { "Hong Kong", 0x27 },
    { "Hungary", 0x2a },
    { "Iceland", 0x31 },
    { "India", 0x2e },
    { "Indonesia", 0x2b },
    { "Iran", 0x30 },
    { "Iraq", 0x2f },
    { "Ireland", 0x2c },
    { "Israel", 0x2d },
    { "Italy", 50 },
    { "Jamaica", 0x33 },
    { "Japan", 0x35 },
    { "Jordan", 0x34 },
    { "Kazakhstan", 0x3a },
    { "Kenya", 0x36 },
    { "Korea", 0x38 },
    { "Kuwait", 0x39 },
    { "Kyrgyzstan", 0x37 },
    { "Latvia", 0x3f },
    { "Lebanon", 0x3b },
    { "Libyan Arab Jamahiriya", 0x40 },
    { "Liechtenstein", 60 },
    { "Lithuania", 0x3d },
    { "Luxembourg", 0x3e },
    { "Macao", 0x45 },
    { "Macedonia", 0x43 },
    { "Malaysia", 0x48 },
    { "Maldives", 70 },
    { "Mexico", 0x47 },
    { "Monaco", 0x42 },
    { "Mongolia", 0x44 },
    { "Morocco", 0x41 },
    { "Netherlands", 0x4a },
    { "New Zealand", 0x4c },
    { "Nicaragua", 0x49 },
    { "Norway", 0x4b },
    { "Oman", 0x4d },
    { "Pakistan", 0x51 },
    { "Panama", 0x4e },
    { "Paraguay", 0x55 },
    { "Peru", 0x4f },
    { "Philippines", 80 },
    { "Poland", 0x52 },
    { "Portugal", 0x54 },
    { "PuertoRico", 0x53 },
    { "Qatar", 0x56 },
    { "Romania", 0x57 },
    { "Russian Federation", 0x58 },
    { "Saudi Arabia", 0x59 },
    { "Singapore", 0x5b },
    { "Slovakia", 0x5d },
    { "Slovenia", 0x5c },
    { "South Africa", 0x6d },
    { "Spain", 0x1f },
    { "Sweden", 90 },
    { "Switzerland", 0x12 },
    { "Syrian Arab Republic", 0x60 },
    { "Taiwan", 0x65 },
    { "Thailand", 0x61 },
    { "TrinidadAndTobago", 100 },
    { "Tunisia", 0x62 },
    { "Turkey", 0x63 },
    { "Ukraine", 0x66 },
    { "United Arab Emirates", 1 },
    { "United Kingdom", 0x23 },
    { "United States", 0x67 },
    { "Unknown", 0 },
    { "Uruguay", 0x68 },
    { "Uzbekistan", 0x69 },
    { "Venezuela", 0x6a },
    { "Vietnam", 0x6b },
    { "Yemen", 0x6c },
    { "Zimbabwe", 110 }
};

struct GameEntry
{
    GameGpd *gpd;
    TitleEntry *titleEntry;
    bool updated;
    string tempFileName;
    string gpdName;
};

struct AvatarAwardGameEntry
{
    GameGpd *gameGpd;
    TitleEntry *titleEntry;
    AvatarAwardGpd *gpd;
    bool updated;
    string tempFileName;
    string gpdName;
};

enum State
{
    StateLocked,
    StateUnlockedOffline,
    StateUnlockedOnline
};

Q_DECLARE_METATYPE( struct AvatarAward* )

class ProfileEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProfileEditor(QStatusBar *statusBar, StfsPackage *profile, bool dispose, QWidget *parent = 0);
    ~ProfileEditor();
    
    bool isOk();
    
private slots:
    void on_gamesList_itemSelectionChanged();

    void on_achievementsList_itemSelectionChanged();

    void replyFinishedBoxArt(QNetworkReply* aReply);

    void replyFinishedAwBoxArt(QNetworkReply* aReply);

    void replyFinishedAwImg(QNetworkReply* aReply);

    void on_aaGamelist_itemSelectionChanged();

    void on_avatarAwardsList_itemSelectionChanged();

    void on_btnUnlockAllAchvs_clicked();

    void on_btnExtractGPD_clicked();

    void on_btnExtractGPD_2_clicked();

    void on_btnUnlockAllAwards_clicked();

    void on_btnCreateAch_clicked();

    void on_cmbxAchState_currentIndexChanged(int index);

    void on_cmbxAwState_currentIndexChanged(int index);

    void on_btnShowAll_clicked();

    void on_btnAwardShowAll_clicked();

    void on_chxPasscode_stateChanged(int arg1);

    void on_chxLIVE_stateChanged(int arg1);

    void onSaveAchievementThumbnail(QPoint p);

    void onSaveAvatarAwardThumbnail(QPoint p);

    void on_dteAchTimestamp_dateTimeChanged(const QDateTime &date);

    void on_dteAwTimestamp_dateTimeChanged(const QDateTime &date);

    void on_txtGameSearch_textChanged(const QString &arg1);

    void on_txtAwardGameSearch_textChanged(const QString &arg1);

    void showAvatarContextMenu(QPoint point);

    void onAssetsDoneDownloading();

    void on_tabWidget_currentChanged(int index);

    void onUnlockEverything();

    void on_cmbxConsoleType_currentIndexChanged(int index);

private:
    Ui::ProfileEditor *ui;
    StfsPackage *profile;
    StfsPackage *PEC;
    QPushButton *unlockEverything;
    QNetworkAccessManager *gameBoxArtManager;
    QNetworkAccessManager *awardBoxArtManager;
    QNetworkAccessManager *awardThumbnailManager;

    GpdUploader *uploader;
    DashboardGpd *dashGpd;
    Account *account;
    AvatarAssetDownloader *downloader;

    vector<string> tempFiles;
    vector<GameEntry> games;
    vector<AvatarAwardGameEntry> aaGames;

    string accountTempPath;

    bool dispose;
    bool ok;
    bool v1Downloaded;
    bool v2Downloaded;

    QStatusBar *statusBar;

    QString v1TempPath, v2TempPath;
    QString assetSavePath;

    void addToDashGpd(SettingEntry *entry, SettingEntryType type, UINT64 id);
    void updateAvatarAward(TitleEntry *entry, AvatarAwardGpd *gpd, struct AvatarAward *award, State toSet);
    void updateAchievement(TitleEntry *entry, AchievementEntry *chiev, State toSet, GameGpd *gpd);
    State getStateFromFlags(DWORD flags);
    void saveAll();
    void loadGameInfo(int index);
    void loadAchievementInfo(int gameIndex, unsigned int chievIndex);
    void loadAwardGameInfo(int index);
    void loadAvatarAwardInfo(int gameIndex, unsigned int awardIndex);
    void showAllGames();
    void showAllAwardGames();
    void saveImage(QPoint p, QLabel *imgLabel);
    void unlockAllAchievements(int index);
    void unlockAllAwards(int index);
};

#endif // PROFILEEDITOR_H
