#pragma once

#include "XDBF.h"
#include "XDBFDefininitions.h"
#include "XDBFHelpers.h"
#include "GPDBase.h"

#include <QVector>

class DashboardGPD : public GPDBase
{
public:
    DashboardGPD(const QString &gpdPath);
	DashboardGPD(FileIO *io);

	~DashboardGPD(void);

    QVector<TitleEntry> gamesPlayed;

	SettingEntry gamercardRegion;
	SettingEntry gamerzone;
	SettingEntry gamerscoreUnlocked;
	SettingEntry achievementsUnlocked;
	SettingEntry reputation;
	SettingEntry gamePlayedCount;
	SettingEntry motto;
	SettingEntry gamerPictureKey;
	SettingEntry gamerBio;
	SettingEntry gamerName;
	SettingEntry gamerLocation;
	SettingEntry avatarInformation;
    ImageEntry avatarImage;
	SettingEntry yearsOnLive;

	// Description: get the box art URL hosted on xbox.com for the title passed in
    static QString GetSmallBoxArtURL(TitleEntry *entry);
    static QString GetLargeBoxArtURL(TitleEntry *entry);

	// Description: write the title entry back to the file
	void WriteTitleEntry(TitleEntry *entry);

	// Description: create a new title entry, make it look like you played a game you actually haven't
	void CreateTitleEntry(TitleEntry *entry);

	// Description: delete a title entry
	void DeleteTitleEntry(TitleEntry *entry);

	// Description: remove all of the unused memory from the gpd
	void CleanGPD();

private:

	// Description: read the title entry
	TitleEntry readTitleEntry(XDBFEntry entry);

	// read all of the starting stuff
	void init();
};

