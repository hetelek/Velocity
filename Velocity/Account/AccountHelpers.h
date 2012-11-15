#pragma once

#include "GPD/XDBFDefininitions.h"
#include "AccountDefinitions.h"

#include <QString>

class AccountHelpers
{
public:
    static QString CountryToString(XboxLiveCountry country);

    static QString SubscriptionTeirToString(SubscriptionTeir teir);

    static QString ConsoleLanguageToString(ConsoleLanguage language);

    static QString XboxLiveServiceProviderToString(XboxLiveServiceProvider provider);
};

