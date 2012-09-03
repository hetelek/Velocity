#pragma once

// other
#include <iostream>
#include <stdio.h>
#include "FileIO.h"
#include "Account/AccountDefinitions.h"
#include "GPD/XDBFDefininitions.h"
#include "Stfs/StfsConstants.h"

// botan
#include <botan/botan.h>
#include <botan/sha160.h>
#include <botan/hmac.h>
#include <botan/arc4.h>

using std::string;
using std::wstring;

class Account
{
public:
    Account(string path, bool decrypt = true, ConsoleType type = Retail);
	~Account(void);

	// Description: returns true if the player has a passcode, false otherwise
	bool IsPasscodeEnabled();

	// Description: returns true if the account is for XBL
	bool IsLiveEnabled();

	// Description: returns true if the account is in the process of being recovered
	bool IsRecovering();

	// Description: returns true if parental controls are enabled
	bool IsParentalControlled();

	// Description: returns true if the player pays via a credit card
	bool IsPaymentInstrumentCreditCard();

	// Description: returns true if the XUID is for offline
	bool IsXUIDOffline();

	// Description: returns true if the XUID is for online
	bool IsXUIDOnline();

	// Description: returns true if the XUID is valid
	bool IsValidXUID();

	// Description: not sure, probably something to do with developers
	bool IsTeamXUID();

	// Description: returns the type of subscription the player has
	SubscriptionTeir GetSubscriptionTeir();

	// Description: returns the country that the player lives in
	XboxLiveCountry GetCountry();

	// Description: returns the langauge used by the player
	ConsoleLanguage GetLanguage();

	// Description: get the player's gamertag
	wstring GetGamertag();

	// Description: get the player's XUID
	UINT64 GetXUID();

	// Description: get the player's xbl service provider
	XboxLiveServiceProvider GetServiceProvider();

	// Description: get the player's passcode, if it isn't enabled, then the values won't be returned
	void GetPasscode(BYTE *passcode);

	// Description: get the onlne domain for XboxLIVE
	string GetOnlineDomain();

	// Description: get the kerbros realm for XboxLIVE
	string GetKerbrosRealm();

	// Description: get the online key for the player
	void GetOnlineKey(BYTE *outKey);

	// Description: enable/disable a passcode
	void SetPasscodeEnabled(bool b);

	// Description: enable/disable connections to XboxLIVE
	void SetLiveEnabled(bool b);

	// Description: enable/disable account in process of recovering
	void SetRecovering(bool b);

	// Description: enable/disable parental controlls
	void SetParentalControlled(bool b);

	// Description: enable/disable payment method as a credit card
	void SetPaymentInstrumentCreditCard(bool b);

	// Description: set the subscription teir for the player
	void SetSubscriptionTeir(SubscriptionTeir teir);

	// Description: set the country that the player lives in
	void SetCountry(XboxLiveCountry country);

	// Description: set the language for the player
	void SetLanguage(ConsoleLanguage language);

	// Description: set the gamertag for the player
	void SetGamertag(wstring gamertag);

private:
	FileIO *io;
	bool ioPassedIn;
	AccountInfo account;
    std::string outPath;
    ConsoleType type;

	void parseFile();

	void writeFile();

    FileIO* decryptAccount(std::string *outPath, ConsoleType type);
};

