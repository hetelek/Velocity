#pragma once

#include "XDBF.h"
#include "XDBFDefininitions.h"
#include "XDBFHelpers.h"
#include "GPDBase.h"
#include <iostream>

using std::string;

class GameGPD : public GPDBase
{
public:
	GameGPD(string gpdPath);
	GameGPD(FileIO *io);

	~GameGPD(void);

	// Description: all of the achievements in this gpd
	vector<AchievementEntry> achievements;

	// Description: the name of the game the achievement is for
	StringEntry gameName;

	// Description: the thumbnail image for the game
	ImageEntry thumbnail;

	// Description: write an achievement entry back to the gpd
	void WriteAchievementEntry(AchievementEntry *entry);

	// Description: create an achievement 
	void CreateAchievement(AchievementEntry *entry, BYTE *thumbnail, DWORD thumbnailLen);

	// Description: delete an achievement
	void DeleteAchievement(AchievementEntry *entry);

	// Description: get the type of an achievement
    static QString GetAchievementType(AchievementEntry *entry);

	// Decription: retrieves the thumbnail image for the achievement passed in, returns true if found and false if not
	bool GetAchievementThumbnail(AchievementEntry *entry, ImageEntry *out);

	// Description: unlock all of the achievements in the GPD offline and write them to the file
	void UnlockAllAchievementsOffline();

	// Description: remove all of the unused memory from the gpd
	void CleanGPD();

private:

	// Descritpion: read the achievement entry passed in
	AchievementEntry readAchievementEntry(XDBFEntry entry);

	// Description: read in all of the default stuff to be read in for a game gpd
	void init();
};

