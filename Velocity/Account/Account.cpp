#include "Account.h"

Account::Account(string path, bool decrypt, ConsoleType type) : ioPassedIn(false), type(type)
{
    Botan::LibraryInitializer init;
	io = new FileIO(path);

    if (decrypt)
    {
        io->close();
        io = decryptAccount(&outPath, type);
    }

	parseFile();
}

void Account::parseFile()
{
	// seek to the begining of the file
	io->setPosition(0);

	// read the data
	account.reservedFlags = io->readDword();
	account.liveFlags = io->readDword();
	account.gamertag = io->readWString(16);

	io->setPosition(0x28);
	account.xuid = io->readUInt64();
	account.cachedUserFlags = io->readDword();
	account.serviceProvider = (XboxLiveServiceProvider)io->readDword();

	account.passcode[0] = io->readByte();
	account.passcode[1] = io->readByte();
	account.passcode[2] = io->readByte();
	account.passcode[3] = io->readByte();

	account.onlineDomain = io->readString(20);
	io->setPosition(0x50);
	account.kerbrosRealm = io->readString(24);
	
	io->readBytes(account.onlineKey, 0x10);
}

bool Account::IsPasscodeEnabled()
{
	return (bool)(account.reservedFlags & PasswordProtected);
}

bool Account::IsLiveEnabled()
{
	return (bool)(account.reservedFlags & LiveEnabled);
}

bool Account::IsRecovering()
{
	return (bool)(account.reservedFlags & Recovering);
}

bool Account::IsParentalControlled()
{
	return (bool)(account.cachedUserFlags & 0x1000000);
}

bool Account::IsPaymentInstrumentCreditCard()
{
	return (bool)(account.cachedUserFlags & 1);
}

bool Account::IsXUIDOffline()
{
	return ((account.xuid >> 60) & 0xF) == 0xE;
}

bool Account::IsXUIDOnline()
{
	return ((account.xuid >> 48) & 0xFFFF) == 9;
}

bool Account::IsValidXUID()
{
	return IsXUIDOffline() != IsXUIDOnline();
}

bool Account::IsTeamXUID()
{
	return (account.cachedUserFlags & 0xFF00000000000140) == 0xFE00000000000100;
}

void Account::SetPasscodeEnabled(bool b)
{
	if (b)
		account.cachedUserFlags |= PasswordProtected;
	else
		account.cachedUserFlags &= (~PasswordProtected);
}

void Account::SetLiveEnabled(bool b)
{
	if (b)
		account.reservedFlags |= LiveEnabled;
	else
	{
		account.reservedFlags &= (~LiveEnabled);
        account.serviceProvider |= LiveDisasbled;
	}
}

void Account::SetRecovering(bool b)
{
	if (b)
		account.cachedUserFlags |= Recovering;
	else
		account.cachedUserFlags &= (~Recovering);
}

void Account::SetParentalControlled(bool b)
{
	if (b)
		account.cachedUserFlags |= 0x1000000;
	else
		account.cachedUserFlags &= (~0x1000000);
}

void Account::SetPaymentInstrumentCreditCard(bool b)
{
	if (b)
		account.cachedUserFlags |= 1;
	else
		account.cachedUserFlags &= 0xFFFFFFFE;
}

void Account::SetSubscriptionTeir(SubscriptionTeir teir)
{
	account.cachedUserFlags &= (~0xF00000);
	account.cachedUserFlags |= (teir << 20);
}

void Account::SetCountry(XboxLiveCountry country)
{
	account.cachedUserFlags &= (~0xFF00);
	account.cachedUserFlags |= (country << 8);
}

void Account::SetLanguage(ConsoleLanguage language)
{
	account.cachedUserFlags &= (~0x3E000000);
	account.cachedUserFlags |= (language << 25);
}

void Account::SetGamertag(wstring gamertag)
{
    account.gamertag = gamertag;
}

FileIO* Account::decryptAccount(std::string *outPath, ConsoleType type)
{
    Botan::byte hmacHash[0x10];
    Botan::byte rc4Key[0x10];

    io->setPosition(0);
    io->readBytes(hmacHash, 0x10);

    Botan::SHA_160 sha1;
    Botan::HMAC hmacSha1(&sha1);

    if (type == Retail)
        hmacSha1.set_key(RETAIL_KEY, 0x10);
    else
        hmacSha1.set_key(DEVKIT_KEY, 0x10);

    hmacSha1.update(hmacHash, 0x10);
    hmacSha1.final(rc4Key);

    BYTE restOfFile[0x184];
    BYTE confounder[8];
    BYTE payload[0x17C];

    io->readBytes(restOfFile, 0x184);

    Botan::ARC4 rc4;
    rc4.set_key(rc4Key, 0x10);
    rc4.decrypt(restOfFile, 0x184);

    memcpy(confounder, restOfFile, 8);
    memcpy(payload, &restOfFile[8], 0x17C);

    *outPath = std::string(tmpnam(NULL));
    FileIO *decrypted = new FileIO(*outPath, true);
    decrypted->write(payload, 0x17C);
    return decrypted;
}

void Account::writeFile()
{
	// seek to the beginning of the file
	io->setPosition(0);

	// write the information
	io->write(account.reservedFlags);
	io->write(account.liveFlags);
	io->write(account.gamertag);

	io->setPosition(0x28);
	io->write(account.xuid);
	io->write(account.cachedUserFlags);
	io->write(account.serviceProvider);

	io->write(account.passcode[0]);
	io->write(account.passcode[1]);
	io->write(account.passcode[2]);
	io->write(account.passcode[3]);

	io->write(account.onlineDomain);

	io->setPosition(0x50);
	io->write(account.kerbrosRealm);

	io->setPosition(0x68);
	io->write(account.onlineKey, 0x10);
}

SubscriptionTeir Account::GetSubscriptionTeir()
{
	return (SubscriptionTeir)((account.cachedUserFlags & 0xF00000) >> 20);
}

XboxLiveCountry Account::GetCountry()
{
	return (XboxLiveCountry)((account.cachedUserFlags & 0xFF00) >> 8);
}

ConsoleLanguage Account::GetLanguage()
{
	return (ConsoleLanguage)((account.cachedUserFlags & 0x3E000000) >> 25);
}

XboxLiveServiceProvider Account::GetServiceProvider()
{
	return account.serviceProvider;
}

void Account::GetPasscode(BYTE *passcode)
{
	if (!IsPasscodeEnabled())
		return;
	memcpy(passcode, account.passcode, 4);
}

string Account::GetOnlineDomain()
{
	return account.onlineDomain;
}

string Account::GetKerbrosRealm()
{
	return account.kerbrosRealm;
}

void Account::GetOnlineKey(BYTE *outKey)
{
	memcpy(outKey, account.onlineKey, 0x10);
}

wstring Account::GetGamertag()
{
	return account.gamertag;
}

Account::~Account(void)
{
    delete io;
    remove(outPath.c_str());
}
