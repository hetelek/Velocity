#include "GameGPD.h"


GameGPD::GameGPD(string filePath) : GPDBase(filePath)
{
	init();
}

GameGPD::GameGPD(FileIO *io) : GPDBase(io)
{
	init();
}

void GameGPD::CleanGPD()
{
	xdbf->Clean();
    io = xdbf->io;
}

AchievementEntry GameGPD::readAchievementEntry(XDBFEntry entry)
{
	// ensure that the entry passed in is an achievement entry
	if (entry.type != Achievement)
		throw string("GPD: Error reading achievement entry. Specified entry isn't an achievement.\n");

	// seek to the begining of the achievement entry
	io->setPosition(xdbf->GetRealAddress(entry.addressSpecifier));

	// read the entry
	AchievementEntry toReturn;
	toReturn.entry = entry;

	toReturn.structSize = io->readDword();
	toReturn.achievementID = io->readDword();
	toReturn.imageID = io->readDword();
	toReturn.gamerscore = io->readDword();
	toReturn.flags = io->readDword();
	
	FILETIME timeStamp = { io->readDword(), io->readDword() };
	toReturn.unlockTime = XDBFHelpers::FILETIMEtoTimeT(timeStamp);

	toReturn.name = io->readWString();
	toReturn.unlockedDescription = io->readWString();
	toReturn.lockedDescription = io->readWString();

	toReturn.initialLength = 0x1C + ((toReturn.name.size() + toReturn.unlockedDescription.size() + toReturn.lockedDescription.size() + 3) * 2);

	return toReturn;
}

void GameGPD::WriteAchievementEntry(AchievementEntry *entry)
{
	DWORD calculatedLength = 0x1C + ((entry->name.size() + entry->unlockedDescription.size() + entry->lockedDescription.size() + 3) * 2);
	if (calculatedLength != entry->initialLength)
	{
		// adjust the memory if the length changed
		xdbf->DeallocateMemory(xdbf->GetRealAddress(entry->entry.addressSpecifier), entry->entry.length);
		entry->entry.length = calculatedLength;
		entry->entry.addressSpecifier = xdbf->GetSpecifier(xdbf->AllocateMemory(entry->entry.length));
	}

	io->flush();

	// seek to the begining of the entry
	io->setPosition(xdbf->GetRealAddress(entry->entry.addressSpecifier));

	// write the entry 
	io->write(entry->structSize);
	io->write(entry->achievementID);
	io->write(entry->imageID);
	io->write(entry->gamerscore);
	io->write(entry->flags);

	// write the unlocked time
	if (entry->unlockTime == 0)
		io->write((UINT64)0);
	else
	{
		FILETIME time = XDBFHelpers::TimeTtoFILETIME(entry->unlockTime);
		io->write(time.dwHighDateTime);
		io->write(time.dwLowDateTime);
	}

	// write the strings
	io->write(entry->name);
	io->write(entry->unlockedDescription);
	io->write(entry->lockedDescription);

	xdbf->UpdateEntry(&entry->entry);

	io->flush();
}

void GameGPD::CreateAchievement(AchievementEntry *entry, BYTE *thumbnail, DWORD thumbnailLen)
{
	DWORD entryLen = 0x1C + ((entry->name.size() + entry->unlockedDescription.size() + entry->lockedDescription.size() + 3) * 2);
	entry->initialLength = entryLen;

    // get the next id for the achievement
    int max = 0;
    for (DWORD i = 0; i < achievements.size(); i++)
        if (achievements.at(i).achievementID > max)
            max = achievements.at(i).achievementID;
    entry->achievementID = max + 1;

	// create a new xdbf entry
	entry->entry = xdbf->CreateEntry(Achievement, entry->achievementID, entryLen);

	// add the entry to the vector
	achievements.push_back(*entry);

	// write the achievement to the file
	WriteAchievementEntry(entry);

	// make sure an image was provided
	if (thumbnail == NULL)
		return;

	// make sure that the image is a PNG
	if (*(DWORD*)thumbnail != 0x474E5089)
		throw string("GPD: Error creating image entry. Image must be a PNG.\n");

	// create a new image entry for the thumbnail
	XDBFEntry imageEntry = xdbf->CreateEntry(Image, entry->imageID, thumbnailLen);

	// write the image to the file
	io->setPosition(xdbf->GetRealAddress(imageEntry.addressSpecifier));
	io->write(thumbnail, thumbnailLen);

    // make a copy of the image
    BYTE *imageCopy = new BYTE[thumbnailLen];
    memcpy(imageCopy, thumbnail, thumbnailLen);

    // add the image to the vector
    ImageEntry image;
    image.image = imageCopy;
    image.length = thumbnailLen;
    image.entry = imageEntry;

    images.push_back(image);

    xdbf->UpdateEntry(&entry->entry);
    xdbf->UpdateEntry(&imageEntry);
}

string GameGPD::GetAchievementType(AchievementEntry *entry)
{
    return XDBFHelpers::AchievementTypeToString((AchievementFlags)(entry->flags & 7));
}

void GameGPD::DeleteAchievement(AchievementEntry *entry)
{
	// remove the entry from the list
	DWORD i;
	for (i = 0 ; i < achievements.size(); i++)
	{
		if (achievements.at(i).entry.id == entry->entry.id)
		{
			achievements.erase(achievements.begin() + i);
			break;
		}
	}
	if (i == achievements.size())
		throw string("GPD: Error deleting achievement. Achievement doesn't exist.\n");

	// delete the entry from the file
	xdbf->DeleteEntry(entry->entry);
}

bool GameGPD::GetAchievementThumbnail(AchievementEntry *entry, ImageEntry *out)
{
	for (DWORD i = 0; i < images.size(); i++)
	{
		if (images.at(i).entry.id == entry->imageID)
		{
			*out = images.at(i);
			return true;
		}
	}
	return false;
}

void GameGPD::UnlockAllAchievementsOffline()
{
	// iterate through all of the achievements
	for (DWORD i = 0; i < achievements.size(); i++)
	{
		// unlock the achievement
		achievements.at(i).flags |= Unlocked;

		// write the updated flags to the entry
		io->setPosition(xdbf->GetRealAddress(achievements.at(i).entry.addressSpecifier) + 0x10);
		io->write(achievements.at(i).flags);

		// update the sync stuff
		xdbf->UpdateEntry(&achievements.at(i).entry);
    }
}

void GameGPD::init()
{
	// read all of the achievement entries
	for (DWORD i = 0; i < xdbf->achievements.entries.size(); i++)
		achievements.push_back(readAchievementEntry(xdbf->achievements.entries.at(i)));

	// find the thumbnail for the game
	for (DWORD i = 0; i < images.size(); i++)
		if (images.at(i).entry.id == 0x8000)
			thumbnail = images.at(i);

	// find the game name string
	for (DWORD i = 0; i < strings.size(); i++)
		if (strings.at(i).entry.id == 0x8000)
			gameName = strings.at(i);
}

GameGPD::~GameGPD(void)
{
    if (!ioPassedIn)
       io->close();
}
