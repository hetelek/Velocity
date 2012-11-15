#include "Account.h"

#include <QString>
#include <QFile>

// other
#include <iostream>
#include <stdio.h>
#include "FileIO.h"

// botan
#include <botan/botan.h>
#include <botan/sha160.h>
#include <botan/hmac.h>
#include <botan/arc4.h>


Account::Account(const QString &path, bool decrypt, ConsoleType type) : ioPassedIn(false), decrypt(decrypt), path(path), type(type)
{
    Botan::LibraryInitializer init;

    if (decrypt)
    {
        decryptAccount(path, &outPath, type);
        io = new FileIO(outPath.toStdString());
    }
    else
    {
        io = new FileIO(path.toStdString());
        outPath = path;
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

    io->setPosition(0x38);
	account.passcode[0] = io->readByte();
	account.passcode[1] = io->readByte();
	account.passcode[2] = io->readByte();
	account.passcode[3] = io->readByte();

    account.onlineDomain = QString::fromStdString(io->readString(20));
	io->setPosition(0x50);
    account.kerbrosRealm = QString::fromStdString(io->readString(24));
	
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
    return (account.xuid & 0xFF00000000000140) == 0xFE00000000000100;
}

UINT64 Account::GetXUID()
{
    return account.xuid;
}

void Account::SetPasscodeEnabled(bool b)
{
	if (b)
        account.reservedFlags |= PasswordProtected;
	else
        account.reservedFlags &= (~PasswordProtected);
}

void Account::SetLiveEnabled(bool b)
{
	if (b)
		account.reservedFlags |= LiveEnabled;
	else
	{
		account.reservedFlags &= (~LiveEnabled);
        account.serviceProvider = LiveDisabled;
	}
}

void Account::SetRecovering(bool b)
{
	if (b)
        account.reservedFlags |= Recovering;
	else
        account.reservedFlags &= (~Recovering);
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

void Account::SetXUIDOnline()
{
    account.xuid = (account.xuid & 0xFFFFFFFFFFFF) | 0x9000000000000;
}

void Account::SetXUIDOffline()
{
    account.xuid = 0;
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

void Account::SetGamertag(std::wstring gamertag)
{
    account.gamertag = gamertag;
}

void Account::SetPasscode(BYTE *passcode)
{
    memcpy(account.passcode, passcode, 4);
}

void Account::SetOnlineServiceProvider(XboxLiveServiceProvider provider)
{
    account.serviceProvider = provider;
}

void Account::Save(ConsoleType type)
{
    writeFile();
    io->close();
    encryptAccount(outPath, type, &path);
}

void Account::decryptAccount(const QString &encryptedPath, QString *outPath, ConsoleType type)
{
    // open the encrypted file
    FileIO encIo(encryptedPath.toStdString());
    BYTE hmacHash[0x10];
    BYTE rc4Key[0x14];

    // read the hash
    encIo.readBytes(hmacHash, 0x10);

    Botan::SHA_160 *sha1 = new Botan::SHA_160;
    Botan::HMAC hmacSha1(sha1);

    // set the hmac-sha1 key
    if (type == Retail)
        hmacSha1.set_key(RETAIL_KEY, 0x10);
    else
        hmacSha1.set_key(DEVKIT_KEY, 0x10);

    // calculate the hmac
    hmacSha1.update(hmacHash, 0x10);
    hmacSha1.final(rc4Key);

    BYTE restOfFile[0x184];
    BYTE confounder[8];
    BYTE payload[0x17C];

    // read the rest of the file
    encIo.readBytes(restOfFile, 0x184);

    // decrypt using rc4
    Botan::ARC4 rc4;
    rc4.set_key(rc4Key, 0x10);

#if defined __unix | defined __APPLE__
    rc4.cipher(restOfFile, restOfFile, 0x184);
#elif _WIN32
    rc4.encrypt(restOfFile, 0x184);
#endif

    // copy the parts
    memcpy(confounder, restOfFile, 8);
    memcpy(payload, &restOfFile[8], 0x17C);

    hmacSha1.update(confounder, 8);
    hmacSha1.update(payload, 0x17C);

    BYTE confoundPayloadHash[0x14];
    hmacSha1.final(confoundPayloadHash);

    if (memcmp(confoundPayloadHash, hmacHash, 0x10) != 0)
        throw QString("Account: Account decryption failed.\n");


    // write the payload
    *outPath = QString(tmpnam(NULL));
    FileIO decrypted(outPath->toStdString(), true);
    decrypted.write(payload, 0x17C);
    decrypted.flush();

    // cleanup
    decrypted.close();
    encIo.close();
}

void Account::encryptAccount(const QString &decryptedPath, ConsoleType type, QString *outPath)
{
    FileIO decIo(decryptedPath.toStdString());
    BYTE decryptedData[0x184];

    // copy in the the confounder
    memcpy(decryptedData, CONFOUNDER, 8);

    // read the decrypted data
    decIo.readBytes(&decryptedData[8], 0x17C);

    // initialization
    Botan::SHA_160 *sha1 = new Botan::SHA_160;
    Botan::HMAC hmacSha1(sha1);

    // set the hmac-sha1 key
    if (type == Retail)
        hmacSha1.set_key(RETAIL_KEY, 0x10);
    else
        hmacSha1.set_key(DEVKIT_KEY, 0x10);

    // hash the confounder and decrypted data
    BYTE hmacHash[0x14];
    hmacSha1.update(decryptedData, 0x184);
    hmacSha1.final(hmacHash);

    // begin writing the payload
    if (outPath == NULL)
        *outPath = QString(tmpnam(NULL));
    FileIO encrypted(outPath->toStdString(), true);
    encrypted.write(hmacHash, 0x10);

    // generate the rc4 key
    BYTE rc4Key[0x14];
    hmacSha1.update(hmacHash, 0x10);
    hmacSha1.final(rc4Key);

    // encrypt the data
    Botan::ARC4 rc4;
    rc4.set_key(rc4Key, 0x10);

#if defined __unix | defined __APPLE__
    rc4.cipher1(decryptedData, 0x184);
#elif _WIN32
    rc4.decrypt(decryptedData, 0x184);
#endif

    // write the confounder and encrypted data
    encrypted.write(decryptedData, 0x184);
    encrypted.flush();

    // clean up
    encrypted.close();
    decIo.close();
}

void Account::writeFile()
{
	// seek to the beginning of the file
	io->setPosition(0);

	// write the information
	io->write(account.reservedFlags);
	io->write(account.liveFlags);
	io->write(account.gamertag);

    // null the bytes out after the gamertag
    io->setPosition(8 + (account.gamertag.length() * 2));
    BYTE temp[0x20] = {0};
    io->write(temp, 0x20 - (account.gamertag.length() * 2));

	io->setPosition(0x28);
	io->write(account.xuid);
	io->write(account.cachedUserFlags);
	io->write(account.serviceProvider);

    io->setPosition(0x38);

	io->write(account.passcode[0]);
	io->write(account.passcode[1]);
	io->write(account.passcode[2]);
	io->write(account.passcode[3]);

    io->write(account.onlineDomain.toStdString());

	io->setPosition(0x50);
    io->write(account.kerbrosRealm.toStdString());

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
    {
        memset(passcode, 0, 4);
        return;
    }
    memcpy(passcode, account.passcode, 4);
}

QString Account::GetOnlineDomain()
{
    return account.onlineDomain;
}

QString Account::GetKerbrosRealm()
{
    return account.kerbrosRealm;
}

void Account::GetOnlineKey(BYTE *outKey)
{
	memcpy(outKey, account.onlineKey, 0x10);
}

std::wstring Account::GetGamertag()
{
	return account.gamertag;
}

Account::~Account(void)
{
    io->close();
    delete io;
    QFile::remove(outPath);
}
