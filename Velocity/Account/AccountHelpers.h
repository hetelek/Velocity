#pragma once

#include "AccountDefinitions.h"
#include "GPD/XDBFHelpers.h"
#include "GPD/XDBFDefininitions.h"
#include "FileIO.h"
#include <iostream>

using std::string;

class AccountHelpers
{
public:
	static string CountryToString(XboxLiveCountry country);

	static string SubscriptionTeirToString(SubscriptionTeir teir);

	static string ConsoleLanguageToString(ConsoleLanguage language);

	static string XboxLiveServiceProviderToString(XboxLiveServiceProvider provider);
};

