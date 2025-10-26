#pragma once

#include "AccountDefinitions.h"
#include <XboxInternals/Gpd/XdbfHelpers.h>
#include <XboxInternals/Gpd/XdbfDefinitions.h>
#include <XboxInternals/IO/FileIO.h>
#include <iostream>
#include <stdlib.h>

#include <XboxInternals/Export.h>

using std::string;

class XBOXINTERNALS_EXPORT AccountHelpers
{
public:
    static string CountryToString(XboxLiveCountry country);

    static string SubscriptionTierToString(SubscriptionTier tier);

    static string ConsoleLanguageToString(ConsoleLanguage language);

    static string XboxLiveServiceProviderToString(XboxLiveServiceProvider provider);
};



