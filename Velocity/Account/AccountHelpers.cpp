#include "AccountHelpers.h"

#include "GPD/XDBFHelpers.h"

#include "FileIO.h"

QString AccountHelpers::CountryToString(XboxLiveCountry country)
{
    return XDBFHelpers::RegionIDtoString(country);
}

QString AccountHelpers::SubscriptionTeirToString(SubscriptionTeir teir)
{
	switch (teir)
	{
		case NoSubcription:
            return QString("No Subcription");
		case Silver:
            return QString("Silver");
		case Gold:
            return QString("Gold");
		case FamilyGold:
            return QString("FamilyGold");
		default:
            throw QString("Account: Invalid 'Subscription Teir' value.\n");
	}
}

QString AccountHelpers::XboxLiveServiceProviderToString(XboxLiveServiceProvider provider)
{
	switch (provider)
	{
        case LiveDisabled:
            return QString("Live Disabled");
		case ProductionNet:
            return QString("ProductionNet");
		case PartnerNet:
            return QString("PartnerNet");
		default:
			char temp[5] = {0};
			memcpy(temp, &provider, 4);
			FileIO::swapEndian(temp, 1, 4);
            return QString(temp);
	}
}

QString AccountHelpers::ConsoleLanguageToString(ConsoleLanguage language)
{
	switch (language)
	{
		case NoLanguage:
            return QString("No Language");
		case English:
            return QString("English");
		case Japanese:
            return QString("Japanese");
		case German:
            return QString("German");
		case French:
            return QString("French");
		case Spanish:
            return QString("Spanish");
		case Italian:
            return QString("Italian");
		case Korean:
            return QString("Korean");
		case TChinese:
            return QString("TChinese");
		case Portuguese:
            return QString("Portuguese");
		case SChinese:
            return QString("SChinese");
		case Polish:
            return QString("Polish");
		case Russian:
            return QString("Russian");
		default:
            throw QString("Account: Invalid 'ConsoleLanguage' value.\n");
	}
}
