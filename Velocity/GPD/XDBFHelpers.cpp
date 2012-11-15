#include "GPD/XDBFHelpers.h"
#include "AvatarAsset/AssetHelpers.h"
#include <sstream>

#include <QString>

using std::stringstream;

QString XDBFHelpers::EntryTypeToString(EntryType type)
{
	if (type < 1 || type > 6)
        throw QString("XDBF: Invalid entry type.\n");

    QString strings[6] = { "Achievement", "Image", "Setting", "Title", "String", "Avatar Award" };
    return strings[type - 1];
}

QString XDBFHelpers::SettingEntryTypeToString(SettingEntryType type)
{
	if (type <= 0 || type > 7)
        throw QString("XDBF: Invalid setting entry type.\n");

    QString strings[7] = { "Int32", "Int64", "Double", "Unicode String", "Float", "Binary", "DataTime" };
	return strings[type - 1];
}

bool XDBFHelpers::CompareEntries(XDBFEntry a, XDBFEntry b)
{
	if (a.type != b.type)
		return a.type < b.type;
	else
		return a.id < b.type;
}

time_t XDBFHelpers::FILETIMEtoTimeT(FILETIME time)
{
    INT64 i64 = (((INT64)(time.dwHighDateTime)) << 32) + time.dwLowDateTime;
    return (time_t)((i64 - 116444736000000000) / 10000000);
}

FILETIME XDBFHelpers::TimeTtoFILETIME(time_t time)
{
    FILETIME toReturn;

    UINT64 ll = ((UINT64)(time * (UINT64)10000000)) + 116444736000000000;
    toReturn.dwLowDateTime = (DWORD)ll;
    toReturn.dwHighDateTime = ll >> 32;
	return toReturn;
}

QString XDBFHelpers::GetAchievementState(AchievementEntry *entry)
{
    if (entry->flags & UnlockedOnline)
        return "Unlocked Online";
    else if (entry->flags & Unlocked)
        return "Unlocked Offline";
    else
        return "Locked";
}

QString XDBFHelpers::AchievementTypeToString(AchievementFlags type)
{
	switch (type)
	{
        case Completion:
            return QString("Completion");
        case Leveling:
            return QString("Leveling");
        case Unlock:
            return QString("Unlock");
        case Event:
            return QString("Event");
        case Tournament:
            return QString("Tournament");
        case Checkpoint:
            return QString("Checkpoint");
        case Other:
            return QString("Other");
        default:
            throw QString("GPD: Invalid achievement type.\n");
	}
}

QString XDBFHelpers::AssetSubcategoryToString(AssetSubcategory category)
{
	return AssetHelpers::AssetSubcategoryToString(category);
}

QString XDBFHelpers::AssetGenderToString(AssetGender gender)
{
	return AssetHelpers::AssetGenderToString(gender);
}

QString XDBFHelpers::IDtoString(UINT64 id)
{
	switch (id)
	{
		case WebConnectionSpeed:
            return QString("Web Connection Speed");
		case WebEmailFormat:
            return QString("Web Email Format");
		case WebFavoriteGame:
            return QString("Web Favorite Game");
		case WebFavoriteGame1:
            return QString("Web Favorite Game1");
		case WebFavoriteGame2:
            return QString("Web Favorite Game 2");
		case WebFavoriteGame3:
            return QString("Web Favorite Game 3");
		case WebFavoriteGame4:
            return QString("Web Favorite Game 4");
		case WebFavoriteGame5:
            return QString("Web Favorite Game 5");
		case WebFavoriteGenre:
            return QString("Web Favorite Genre");
		case WebFlags:
            return QString("Web Flags");
		case WebFlash:
            return QString("Web Flash");
		case WebPlatformsOwned:
            return QString("Web Platforms Owned");
		case WebSpam:
            return QString("Web Spam");
		case WebVideoPreference:
            return QString("Web Video Preference");
		case CruxBgLargePublic:
            return QString("Crux Bg Large Public");
		case CruxBgSmallPublic:
            return QString("Crux Bg Small Public");
		case CruxBio:
            return QString("Crux Bio");
		case CruxBkgdImage:
            return QString("Crux Bkgd Image");
		case CruxLastChangeTime:
            return QString("Crux Last Change Time");
		case CruxMediaMotto:
            return QString("Crux Media Motto");
		case CruxMediaPicture:
            return QString("Crux Media Picture");
		case CruxMediaStyle1:
            return QString("Crux Media Style 1");
		case CruxMediaStyle2:
            return QString("Crux Media Style 2");
		case CruxMediaStyle3:
            return QString("Crux Media Style 3");
		case CruxOfflineId:
            return QString("Crux Offline Id");
		case CruxTopAlbum1:
            return QString("Crux Top Album 1");
		case CruxTopAlbum2:
            return QString("Crux Top Album 2");
		case CruxTopAlbum3:
            return QString("Crux Top Album 3");
		case CruxTopAlbum4:
            return QString("Crux Top Album 4");
		case CruxTopAlbum5:
            return QString("Crux Top Album 5");
		case CruxTopMediaid1:
            return QString("Crux Top Mediaid 1");
		case CruxTopMediaid2:
            return QString("Crux Top Mediaid 2");
		case CruxTopMediaid3:
            return QString("Crux Top Mediaid 3");
		case CruxTopMusic:
            return QString("Crux Top Music");
		case FriendsappShowBuddies:
            return QString("Friendsapp Show Buddies");
		case GamerActionAutoAim:
            return QString("Gamer Action Auto Aim");
		case GamerActionAutoCenter:
            return QString("Gamer Action Auto Center");
		case GamerActionMovementControl:
            return QString("Gamer Action Movement Control");
		case GamerControlSensitivity:
            return QString("Gamer Control Sensitivity");
		case GamerDifficulty:
            return QString("Gamer Difficulty");
		case GamerPreferredColorFirst:
            return QString("Gamer Preferred Color First");
		case GamerPreferredColorSecond:
            return QString("Gamer Preferred Color Second");
		case GamerPresenceUserState:
            return QString("Gamer Presence User State");
		case GamerRaceAcceleratorControl:
            return QString("Gamer Race Accelerator Control");
		case GamerRaceBrakeControl:
            return QString("Gamer Race Brake Control");
		case GamerRaceCameraLocation:
            return QString("Gamer Race Camera Location");
		case GamerRaceTransmission:
            return QString("Gamer Race Transmission");
		case GamerTier:
            return QString("Gamer Tier");
		case GamerType:
            return QString("Gamer Type");
		case YearsOnLive:
            return QString("Years On LIVE");
		case GamerYaxisInversion:
            return QString("Gamer Yaxis Inversion");
		case GamercardAchievementsEarned:
            return QString("Gamercard Achievements Earned");
		case GamercardAvatarInfo1:
            return QString("Gamercard Avatar Info 1");
		case GamercardAvatarInfo2:
            return QString("Gamercard Avatar Info 2");
		case GamercardCred:
            return QString("Gamercard Cred");
		case GamercardHasVision:
            return QString("Gamercard Has Vision");
		case GamercardMotto:
            return QString("Gamercard Motto");
		case GamercardPartyInfo:
            return QString("Gamercard Party Info");
		case GamercardPersonalPicture:
            return QString("Gamercard Personal Picture");
		case GamercardPictureKey:
            return QString("Gamercard Picture Key");
		case GamercardRegion:
            return QString("Gamercard Region");
		case GamercardRep:
            return QString("Gamercard Rep");
		case GamercardServiceTypeFlags:
            return QString("Gamercard Service Type Flags");
		case GamercardTitleAchievementsEarned:
            return QString("Gamercard Title Achievements Earned");
		case GamercardTitleCredEarned:
            return QString("Gamercard Title Cred Earned");
		case GamercardTitlesPlayed:
            return QString("Gamercard Titles Played");
		case GamercardUserBio:
            return QString("Gamercard User Bio");
		case GamercardUserLocation:
            return QString("Gamercard User Location");
		case GamercardUserName:
            return QString("Gamercard User Name");
		case GamercardUserUrl:
            return QString("Gamercard User Url");
		case GamercardZone:
            return QString("Gamercard Zone");
		case MessengerAutoSignin:
            return QString("Messenger Auto Signin");
		case MessengerSignupState:
            return QString("Messenger Signup State");
		case OptionControllerVibration:
            return QString("Option Controller Vibration");
		case OptionVoiceMuted:
            return QString("Option Voice Muted");
		case OptionVoiceThruSpeakers:
            return QString("Option Voice Thru Speakers");
		case OptionVoiceVolume:
            return QString("Option Voice Volume");
		case Permissions:
            return QString("Permissions");
		case SaveWindowsLivePassword:
            return QString("Save Windows Live Password");
		case TitleSpecific1:
            return QString("Title Specific 1");
		case TitleSpecific2:
            return QString("Title Specific 2");
		case TitleSpecific3:
            return QString("Title Specific 3");
		case LastOnLIVE:
            return QString("Last On LIVE");
		case TitleInformation:
            return QString("Title Information");
		case AvatarImage:
            return QString("Avatar Image");
		default:
			stringstream ss;
			ss << "0x" << std::hex << id;
            return QString::fromStdString(ss.str());
	}
}

QString XDBFHelpers::RegionIDtoString(XboxLiveCountry id)
{
	switch (id)
	{
		case Albania:
            return QString("Albania");
		case Algeria:
            return QString("Algeria");
		case Argentina:
            return QString("Argentina");
		case Armenia:
            return QString("Armenia");
		case Australia:
            return QString("Australia");
		case Austria:
            return QString("Austria");
		case Azerbaijan:
            return QString("Azerbaijan");
		case Bahrain:
            return QString("Bahrain");
		case Belarus:
            return QString("Belarus");
		case Belgium:
            return QString("Belgium");
		case Belize:
            return QString("Belize");
		case Bolivia:
            return QString("Bolivia");
		case Brazil:
            return QString("Brazil");
		case BruneiDarussalam:
            return QString("Brunei Darussalam");
		case Bulgaria:
            return QString("Bulgaria");
		case Canada:
            return QString("Canada");
		case Chile:
            return QString("Chile");
		case China:
            return QString("China");
		case Colombia:
            return QString("Colombia");
		case CostaRica:
            return QString("CostaRica");
		case Croatia:
            return QString("Croatia");
		case CzechRepublic:
            return QString("Czech Republic");
		case Denmark:
            return QString("Denmark");
		case DominicanRepublic:
            return QString("Dominican Republic");
		case Ecuador:
            return QString("Ecuador");
		case Egypt:
            return QString("Egypt");
		case ElSalvador:
            return QString("ElSalvador");
		case Estonia:
            return QString("Estonia");
		case FaroeIslands:
            return QString("Faroe Islands");
		case Finland:
            return QString("Finland");
		case France:
            return QString("France");
		case Georgia:
            return QString("Georgia");
		case Germany:
            return QString("Germany");
		case Greece:
            return QString("Greece");
		case Guatemala:
            return QString("Guatemala");
		case Honduras:
            return QString("Honduras");
		case HongKong:
            return QString("HongKong");
		case Hungary:
            return QString("Hungary");
		case Iceland:
            return QString("Iceland");
		case India:
            return QString("India");
		case Indonesia:
            return QString("Indonesia");
		case Iran:
            return QString("Iran");
		case Iraq:
            return QString("Iraq");
		case Ireland:
            return QString("Ireland");
		case Israel:
            return QString("Israel");
		case Italy:
            return QString("Italy");
		case Jamaica:
            return QString("Jamaica");
		case Japan:
            return QString("Japan");
		case Jordan:
            return QString("Jordan");
		case Kazakhstan:
            return QString("Kazakhstan");
		case Kenya:
            return QString("Kenya");
		case Korea:
            return QString("Korea");
		case Kuwait:
            return QString("Kuwait");
		case Kyrgyzstan:
            return QString("Kyrgyzstan");
		case Latvia:
            return QString("Latvia");
		case Lebanon:
            return QString("Lebanon");
		case LibyanArabJamahiriya:
            return QString("Libyan Arab Jamahiriya");
		case Liechtenstein:
            return QString("Liechtenstein");
		case Lithuania:
            return QString("Lithuania");
		case Luxembourg:
            return QString("Luxembourg");
		case Macao:
            return QString("Macao");
		case Macedonia:
            return QString("Macedonia");
		case Malaysia:
            return QString("Malaysia");
		case Maldives:
            return QString("Maldives");
		case Mexico:
            return QString("Mexico");
		case Monaco:
            return QString("Monaco");
		case Mongolia:
            return QString("Mongolia");
		case Morocco:
            return QString("Morocco");
		case Netherlands:
            return QString("Netherlands");
		case NewZealand:
            return QString("New Zealand");
		case Nicaragua:
            return QString("Nicaragua");
		case Norway:
            return QString("Norway");
		case Oman:
            return QString("Oman");
		case Pakistan:
            return QString("Pakistan");
		case Panama:
            return QString("Panama");
		case Paraguay:
            return QString("Paraguay");
		case Peru:
            return QString("Peru");
		case Philippines:
            return QString("Philippines");
		case Poland:
            return QString("Poland");
		case Portugal:
            return QString("Portugal");
		case PuertoRico:
            return QString("Puerto Rico");
		case Qatar:
            return QString("Qatar");
		case Romania:
            return QString("Romania");
		case RussianFederation:
            return QString("Russian Federation");
		case SaudiArabia:
            return QString("SaudiArabia");
		case Singapore:
            return QString("Singapore");
		case Slovakia:
            return QString("Slovakia");
		case Slovenia:
            return QString("Slovenia");
		case SouthAfrica:
            return QString("SouthAfrica");
		case Spain:
            return QString("Spain");
		case Sweden:
            return QString("Sweden");
		case Switzerland:
            return QString("Switzerland");
		case SyrianArabRepublic:
            return QString("Syrian Arab Republic");
		case Taiwan:
            return QString("Taiwan");
		case Thailand:
            return QString("Thailand");
		case TrinidadAndTobago:
            return QString("Trinidad And Tobago");
		case Tunisia:
            return QString("Tunisia");
		case Turkey:
            return QString("Turkey");
		case Ukraine:
            return QString("Ukraine");
		case UnitedArabEmirates:
            return QString("United Arab Emirates");
		case UnitedKingdom:
            return QString("United Kingdom");
		case UnitedStates:
            return QString("United States");
		case Unknown:
            return QString("Unknown");
		case Uruguay:
            return QString("Uruguay");
		case Uzbekistan:
            return QString("Uzbekistan");
		case Venezuela:
            return QString("Venezuela");
		case Vietnam:
            return QString("Vietnam");
		case Yemen:
            return QString("Yemen");
		case Zimbabwe:
            return QString("Zimbabwe");
		default:
            throw QString("GPD: Error converting region ID to string, invalid region.\n");
	}
}
