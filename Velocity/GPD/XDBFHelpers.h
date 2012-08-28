#pragma once

#include "winnames.h"
#include "GPD/XDBFDefininitions.h"
#include <iostream>
#include <time.h>

using std::string;

class XDBFHelpers
{
public:

	static string EntryTypeToString(EntryType type);

	static string SettingEntryTypeToString(SettingEntryType type);

	static bool CompareEntries(XDBFEntry a, XDBFEntry b);

	static struct tm* FILETIMEtoTM(FILETIME time);

	static FILETIME TMtoFILETIME(struct tm* time);

    static string AchievementTypeToString(AchievementFlags type);

	static string RegionIDtoString(XboxLiveCountry id);

	static string AssetSubcategoryToString(AssetSubcategory category);

	static string AssetGenderToString(AssetGender gender);

	static string IDtoString(UINT64 id);

    static string GetAchievementState(AchievementEntry *entry);
};
