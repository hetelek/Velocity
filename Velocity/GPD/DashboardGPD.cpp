#include "DashboardGPD.h"
#include <sstream>

using std::stringstream;


DashboardGPD::DashboardGPD(string gpdPath) : GPDBase(gpdPath)
{
	init();
}

DashboardGPD::DashboardGPD(FileIO *io) : GPDBase(io)
{
	init();
}

void DashboardGPD::CleanGPD()
{
	xdbf->Clean();
}

void DashboardGPD::init()
{
	// set all the settings defaultly to zero, so there is a way of checking whether or not
	// the entries were actually read in
	gamercardRegion.entry.type = 
	gamerzone.entry.type = 
	gamerscoreUnlocked.entry.type = 
	achievementsUnlocked.entry.type = 
	reputation.entry.type = 
	gamePlayedCount.entry.type = 
	motto.entry.type = 
	gamerPictureKey.entry.type = 
	gamerName.entry.type = 
	gamerLocation.entry.type = 
	avatarInformation.entry.type = 
	avatarImage.entry.type = 
	yearsOnLive.entry.type =
	gamerBio.entry.type = (EntryType)0;

	// read in all the title entries
	for (DWORD i = 0; i < xdbf->titlesPlayed.entries.size(); i++)
		gamesPlayed.push_back(readTitleEntry(xdbf->titlesPlayed.entries.at(i)));

    // find the avatar image
    for (DWORD i = 0; i < images.size(); i++)
        if (images.at(i).entry.id == AvatarImage)
        {
            avatarImage = images.at(i);
            break;
        }

	// read in all of the IMPOTANT setting entries
	for (DWORD i = 0; i < settings.size(); i++)
	{
		switch (settings.at(i).entry.id)
		{
			case GamercardRegion:
				gamercardRegion = settings.at(i);
				break;
			case GamercardZone:
				gamerzone = settings.at(i);
				break;
			case GamercardCred:
				gamerscoreUnlocked = settings.at(i);
				break;
            case GamercardAchievementsEarned:
				achievementsUnlocked = settings.at(i);
				break;
			case GamercardRep:
				reputation = settings.at(i);
				break;
            case GamercardMotto:
                motto = settings.at(i);
                break;
			case GamercardTitlesPlayed:
				gamePlayedCount = settings.at(i);
				break;
			case GamercardPictureKey:
				gamerPictureKey = settings.at(i);
				break;
			case GamercardUserName:
				gamerName = settings.at(i);
				break;
			case GamercardUserLocation:
				gamerLocation = settings.at(i);
				break;
			case GamercardAvatarInfo1:
				avatarInformation = settings.at(i);
				break;
			case YearsOnLive:
				yearsOnLive = settings.at(i);
				break;
			case GamercardUserBio:
				gamerBio = settings.at(i);
				break;
		}
	}
}

TitleEntry DashboardGPD::readTitleEntry(XDBFEntry entry)
{
	// ensure that the entry is a title entry
	if (entry.type != Title)
		throw string("GPD: Error reading title entry. Specified entry isn't a title.\n");

	TitleEntry toReturn;
	toReturn.entry = entry;

	// seek to title entry position
	io->setPosition(xdbf->GetRealAddress(entry.addressSpecifier));

	// read the entry
	toReturn.titleID = io->readDword();
	toReturn.achievementCount = io->readDword();
	toReturn.achievementsUnlocked = io->readDword();
	toReturn.totalGamerscore = io->readDword();
	toReturn.gamerscoreUnlocked = io->readDword();
	toReturn.achievementsUnlockedOnline = io->readWord();
	toReturn.avatarAwardsEarned = io->readByte();
	toReturn.avatarAwardCount = io->readByte();
	toReturn.maleAvatarAwardsEarned = io->readByte();
	toReturn.maleAvatarAwardCount = io->readByte();
	toReturn.femaleAvatarAwardsEarned = io->readByte();
	toReturn.femaleAvatarAwardCount = io->readByte();
	toReturn.flags = io->readDword();

	// read the last time played
	FILETIME time = { io->readDword(), io->readDword() };
	toReturn.lastPlayed = XDBFHelpers::FILETIMEtoTimeT(time);

	// read the game name
	toReturn.gameName = io->readWString();

	toReturn.initialLength = entry.length;

	return toReturn;
}

string DashboardGPD::GetSmallBoxArtURL(TitleEntry *entry)
{
	stringstream url;
	url << "http://tiles.xbox.com/consoleAssets/";
    url << std::hex << entry->titleID;
	url << "/en-us/smallboxart.jpg";

	return url.str();
}

string DashboardGPD::GetLargeBoxArtURL(TitleEntry *entry)
{
	stringstream url;
	url << "http://tiles.xbox.com/consoleAssets/";
    url << std::hex << entry->titleID;
	url << "/en-us/largeboxart.jpg";

	return url.str();
}

void DashboardGPD::WriteTitleEntry(TitleEntry *entry)
{
	DWORD calculatedLength = 0x28 + ((entry->gameName.size() + 1) * 2);

	if (calculatedLength != entry->initialLength)
	{
		// adjust the memory if the length changed
		xdbf->DeallocateMemory(xdbf->GetRealAddress(entry->entry.addressSpecifier), entry->entry.length);
		entry->entry.length = calculatedLength;
		entry->entry.addressSpecifier = xdbf->GetSpecifier(xdbf->AllocateMemory(entry->entry.length));
	}

	// seek to the position of the title entry
	io->setPosition(xdbf->GetRealAddress(entry->entry.addressSpecifier));

	io->flush();

	// write the title entry
	io->write(entry->titleID);
	io->write(entry->achievementCount);
	io->write(entry->achievementsUnlocked);
	io->write(entry->totalGamerscore);
	io->write(entry->gamerscoreUnlocked);
	io->write(entry->achievementsUnlockedOnline);
	io->write(entry->avatarAwardsEarned);
	io->write(entry->avatarAwardCount);
	io->write(entry->maleAvatarAwardsEarned);
	io->write(entry->maleAvatarAwardCount);
	io->write(entry->femaleAvatarAwardsEarned);
	io->write(entry->femaleAvatarAwardCount);
	io->write(entry->flags);

	// write the time last played
	FILETIME time = XDBFHelpers::TimeTtoFILETIME(entry->lastPlayed);
	io->write(time.dwHighDateTime);
	io->write(time.dwLowDateTime);

	io->write(entry->gameName);

    xdbf->UpdateEntry(&entry->entry);

	io->flush();
}

void DashboardGPD::DeleteTitleEntry(TitleEntry *entry)
{
	// remove the entry from the list
	DWORD i;
	for (i = 0 ; i < gamesPlayed.size(); i++)
	{
		if (gamesPlayed.at(i).entry.id == entry->entry.id)
		{
			gamesPlayed.erase(gamesPlayed.begin() + i);
			break;
		}
	}
    if (i > gamesPlayed.size())
		throw string("GPD: Error deleting title entry. Title doesn't exist.\n");

	// delete the entry from the file
	xdbf->DeleteEntry(entry->entry);
}

void DashboardGPD::CreateTitleEntry(TitleEntry *entry)
{
	entry->initialLength = 0x28 + ((entry->gameName.size() + 1) * 2);

	// create a new xdbf entry
	entry->entry = xdbf->CreateEntry(Title, entry->titleID, 0x28 + ((entry->gameName.size() + 1) * 2));

	// add the title entry to the vector
	gamesPlayed.push_back(*entry);

	// write the data to the entry
	WriteTitleEntry(entry);
}

DashboardGPD::~DashboardGPD(void)
{
}
