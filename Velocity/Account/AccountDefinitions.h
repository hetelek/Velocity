#pragma once

#include <iostream>
#include "winnames.h"

using std::string;
using std::wstring;

enum PasscodeKeys
{
    DPAD_Up,
    DPAD_Down,
    DPAD_Left,
    DPAD_Right,
    X,
    Y,
    LT,
    RT,
    LB,
    RB
};

enum SubscriptionTeir
{
	NoSubcription = 0,
	Silver = 3,
	Gold = 6,
	FamilyGold = 9
};

enum ReservedFlags
{
	PasswordProtected = 0x10000000,
	LiveEnabled = 0x20000000,
	Recovering = 0x40000000
};

enum ConsoleLanguage
{
    NoLanguage,
    English,
    Japanese,
    German,
    French,
    Spanish,
    Italian,
    Korean,
    TChinese,
    Portuguese,
    SChinese,
    Polish,
    Russian
};

enum XboxLiveServiceProvider
{
	LiveDisasbled = 0,
	ProductionNet = 0x50524F44,	// PROD
	PartnerNet = 0x50415254		// PART
};

struct AccountInfo
{
	DWORD reservedFlags;
	DWORD liveFlags;
	wstring gamertag;
	UINT64 xuid;
	DWORD cachedUserFlags;
	XboxLiveServiceProvider serviceProvider;
	BYTE passcode[4];
	string onlineDomain;
	string kerbrosRealm;
	BYTE onlineKey[0x10];
};