#pragma once

#include "winnames.h"
#include "GPD/XDBFDefininitions.h"
#include <iostream>
#include <time.h>

#include "XboxInternals_global.h"

using std::string;

class XBOXINTERNALSSHARED_EXPORT XDBFHelpers
{
public:

	static string EntryTypeToString(EntryType type);

	static string SettingEntryTypeToString(SettingEntryType type);

	static bool CompareEntries(XDBFEntry a, XDBFEntry b);

    static time_t FILETIMEtoTimeT(WINFILETIME time);

    static WINFILETIME TimeTtoFILETIME(time_t time);

    static string AchievementTypeToString(AchievementFlags type);

	static string RegionIDtoString(XboxLiveCountry id);

	static string AssetSubcategoryToString(AssetSubcategory category);

	static string AssetGenderToString(AssetGender gender);

	static string IDtoString(UINT64 id);

    static string GetAchievementState(AchievementEntry *entry);
};
