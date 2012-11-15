#pragma once

#include "Account/AccountDefinitions.h"
#include "GPD/XDBFDefininitions.h"
#include "Stfs/StfsConstants.h"

#include <QString>

class FileIO;

class Account
{
public:
    Account(const QString &path, bool decrypt = true, ConsoleType type = Retail);
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
    std::wstring GetGamertag();

	// Description: get the player's XUID
	UINT64 GetXUID();

	// Description: get the player's xbl service provider
	XboxLiveServiceProvider GetServiceProvider();

	// Description: get the player's passcode, if it isn't enabled, then the values won't be returned
	void GetPasscode(BYTE *passcode);

	// Description: get the onlne domain for XboxLIVE
    QString GetOnlineDomain();

	// Description: get the kerbros realm for XboxLIVE
    QString GetKerbrosRealm();

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
    void SetGamertag(std::wstring gamertag);

    // Description: set the passcode for the player
    void SetPasscode(BYTE *passcode);

    // Description: set the gamer's XUID to offline
    void SetXUIDOffline();

    // Description: set the gamer's XUID to offline
    void SetXUIDOnline();

    // Description: set the service provider for the player
    void SetOnlineServiceProvider(XboxLiveServiceProvider provider);

    // Description: saves all set data and overwrites the current file
    void Save(ConsoleType type = Retail);

private:
	FileIO *io;
    bool ioPassedIn, decrypt;
	AccountInfo account;
    QString outPath, path;
    ConsoleType type;

	void parseFile();

	void writeFile();

    void decryptAccount(const QString &encryptedPath, QString *outPath, ConsoleType type);
    void encryptAccount(const QString &decryptedPath, ConsoleType type, QString *outPath = NULL);
};

