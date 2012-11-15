#include "AvatarAwardGPD.h"
#include <sstream>

#include <QString>

using std::stringstream;

AvatarAwardGPD::AvatarAwardGPD(string gpdPath) : GPDBase(gpdPath)
{
    init();
}

AvatarAwardGPD::AvatarAwardGPD(FileIO *io) : GPDBase(io)
{
    init();
}

void AvatarAwardGPD::CleanGPD()
{
    xdbf->Clean();
}

void AvatarAwardGPD::init()
{
    // read in all of the avatar award entries
    for (DWORD i = 0; i < xdbf->avatarAwards.entries.size(); i++)
        avatarAwards.push_back(readAvatarAwardEntry(xdbf->avatarAwards.entries.at(i)));
}

AssetGender AvatarAwardGPD::GetAssetGender(struct AvatarAward *award)
{
    return (AssetGender)((award->awardFlags & 0x0000000300000000) >> 32);
}

struct AvatarAward AvatarAwardGPD::readAvatarAwardEntry(XDBFEntry entry)
{
    // make sure the entry passed in is an avatar award
    if (entry.type != AvatarAward)
        throw QString("GPD: Error reading avatar award, specified entry isn't an award.\n");

    struct AvatarAward award;
    award.entry = entry;

    // seek to the address of the award
    io->setPosition(xdbf->GetRealAddress(entry.addressSpecifier));

    // read the award
    award.structSize = io->readDword();
    award.clothingType = io->readDword();
    award.awardFlags = io->readUInt64();
    award.titleID = io->readDword();
    award.imageID = io->readDword();
    award.flags = io->readDword();

    // read the unlock time
    FILETIME time = { io->readDword(), io->readDword() };
    award.unlockTime = XDBFHelpers::FILETIMEtoTimeT(time);

    // read the rest of the entry
    award.subcategory = (AssetSubcategory)io->readDword();
    award.colorizable = io->readDword();
    award.name = io->readWString();
    award.unlockedDescription = io->readWString();
    award.lockedDescription = io->readWString();

    // calculate the initial size of the entry
    award.initialSize = 0x2C + ((award.name.size() + award.unlockedDescription.size() + award.lockedDescription.size() + 3) * 2);

    return award;
}

void AvatarAwardGPD::UnlockAllAwards()
{
    // iterate through all of the avtar awards
    for (DWORD i = 0; i < avatarAwards.size(); i++)
    {
        // set it to unlocked
        avatarAwards.at(i).flags |= (Unlocked | 0x100000);

        // write the flags back to the file
        io->setPosition(xdbf->GetRealAddress(avatarAwards.at(i).entry.addressSpecifier) + 0x18);
        io->write(avatarAwards.at(i).flags);

        // update the sync crap
        xdbf->UpdateEntry(&avatarAwards.at(i).entry);
    }

    io->flush();
}

string AvatarAwardGPD::GetGUID(struct AvatarAward *award)
{
    char guid[38];
    WORD *seg = (WORD*)&award->awardFlags;
    sprintf(guid, "%08lx-%04x-%04x-%04x-%04x%08lx", award->clothingType, seg[3], seg[2], seg[1], seg[0], award->titleID);

    return string(guid);
}

string AvatarAwardGPD::getAwardImageURL(struct AvatarAward *award, bool little)
{
    stringstream url;
    url << "http://avatar.xboxlive.com/global/t.";
    url << std::hex << award->titleID << "/avataritem/";

    url << GetGUID(award) << ((little) ? "/64" : "/128");

    return url.str();
}

string AvatarAwardGPD::GetLittleAwardImageURL(struct AvatarAward *award)
{
    return getAwardImageURL(award, true);
}

string AvatarAwardGPD::GetLargeAwardImageURL(struct AvatarAward *award)
{
    return getAwardImageURL(award, false);
}

void AvatarAwardGPD::WriteAvatarAward(struct AvatarAward *award)
{
    DWORD calculatedLength = 0x2C + ((award->name.size() + award->unlockedDescription.size() + award->lockedDescription.size() + 3) * 2);

    if (calculatedLength != award->initialSize)
    {
        // adjust the memory if the length changed
        xdbf->DeallocateMemory(xdbf->GetRealAddress(award->entry.addressSpecifier), award->entry.length);
        award->entry.length = calculatedLength;
        award->entry.addressSpecifier = xdbf->GetSpecifier(xdbf->AllocateMemory(award->entry.length));
    }

    // seek to the position of the award
    io->setPosition(xdbf->GetRealAddress(award->entry.addressSpecifier));

    // write the entry
    io->write(award->structSize);
    io->write(award->clothingType);
    io->write(award->awardFlags);
    io->write(award->titleID);
    io->write(award->imageID);
    io->write(award->flags);

    // write the unlock time
    FILETIME time = XDBFHelpers::TimeTtoFILETIME(award->unlockTime);
    io->write(time.dwHighDateTime);
    io->write(time.dwLowDateTime);

    // write the rest of the entry
    io->write(award->subcategory);
    io->write(award->colorizable);
    io->write(award->name);
    io->write(award->unlockedDescription);
    io->write(award->lockedDescription);

    xdbf->UpdateEntry(&award->entry);

    io->flush();
}

void AvatarAwardGPD::CreateAvatarAward(struct AvatarAward *award)
{
    award->initialSize = 0x2C + ((award->name.size() + award->unlockedDescription.size() + award->lockedDescription.size() + 3) * 2);

    // create a new xdbf entry for the award
    UINT64 entryID = ((UINT64)award->titleID << 32) | ((DWORD)getNextAwardIndex() << 16) | GetAssetGender(award);
    award->entry = xdbf->CreateEntry(AvatarAward, entryID, award->initialSize);

    // write the award to the file
    WriteAvatarAward(award);

    avatarAwards.push_back(*award);
}

void AvatarAwardGPD::DeleteAvatarAward(struct AvatarAward *award)
{
    // remove the entry from the list
    DWORD i;
    for (i = 0 ; i < avatarAwards.size(); i++)
    {
        if (avatarAwards.at(i).entry.id == award->entry.id)
        {
            avatarAwards.erase(avatarAwards.begin() + i);
            break;
        }
    }
    if (i == avatarAwards.size())
        throw QString("GPD: Error deleting avatar award. Award doesn't exist.\n");

    // delete the entry from the file
    xdbf->DeleteEntry(award->entry);
}

WORD AvatarAwardGPD::getNextAwardIndex()
{
    // get the highest index
    WORD highest = 0;
    for (DWORD i = 0; i < xdbf->avatarAwards.entries.size(); i++)
    {
        WORD currentID = (xdbf->avatarAwards.entries.at(i).id & 0xFFFF0000) >> 16;
        if (highest < currentID)
            highest = currentID;
    }

    return highest + 1;
}

AvatarAwardGPD::~AvatarAwardGPD(void)
{
    if (!ioPassedIn)
        io->close();
}
