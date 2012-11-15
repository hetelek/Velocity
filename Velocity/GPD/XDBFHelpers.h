#pragma once

#include "winnames.h"
#include "GPD/XDBFDefininitions.h"
#include <iostream>
#include <time.h>

#include <QString>

class XDBFHelpers
{
public:

    static QString EntryTypeToString(EntryType type);

    static QString SettingEntryTypeToString(SettingEntryType type);

	static bool CompareEntries(XDBFEntry a, XDBFEntry b);

    static time_t FILETIMEtoTimeT(FILETIME time);

    static FILETIME TimeTtoFILETIME(time_t time);

    static QString AchievementTypeToString(AchievementFlags type);

    static QString RegionIDtoString(XboxLiveCountry id);

	static QString AssetSubcategoryToString(AssetSubcategory category);

	static QString AssetGenderToString(AssetGender gender);

    static QString IDtoString(UINT64 id);

    static QString GetAchievementState(AchievementEntry *entry);
};
