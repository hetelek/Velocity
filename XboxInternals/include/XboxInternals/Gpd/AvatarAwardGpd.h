#pragma once

#include <XboxInternals/Gpd/Xdbf.h>
#include <XboxInternals/Gpd/XdbfDefinitions.h>
#include <XboxInternals/Gpd/XdbfHelpers.h>
#include <XboxInternals/Gpd/GpdBase.h>
#include <iostream>
#include <stdio.h>

#include <XboxInternals/Export.h>

using std::string;

class XBOXINTERNALS_EXPORT AvatarAwardGpd : public GpdBase
{
public:
    AvatarAwardGpd(string gpdPath);
    AvatarAwardGpd(FileIO *io);
    ~AvatarAwardGpd(void);

    vector<AvatarAwardData> avatarAwards;

    // Description: get the gender of the avatar award passed in
    static AssetGender GetAssetGender(AvatarAwardData *award);

    // Description: unlock all of the avatar awards in this gpd
    void UnlockAllAwards();

    // Description: get the URL of the image of the award passed in, hosted on xbox.com, 64x64
    static string GetLittleAwardImageURL(AvatarAwardData *award);

    // Description: get the URL of the image of the award passed in, hosted on xbox.com, 128x128
    static string GetLargeAwardImageURL(AvatarAwardData *award);

    // Description: get the GUID of the avatar award, used in URLs on xbox.com
    static string GetGUID(AvatarAwardData *award);

    // Description: Write the avatar award back to the gpd
    void WriteAvatarAward(AvatarAwardData *award);

    // Description: create a new avatar award
    void CreateAvatarAward(AvatarAwardData *award);

    // Description: delete an award
    void DeleteAvatarAward(AvatarAwardData *award);

    // Description: remove all of the unused memory from the gpd
    void CleanGpd();

private:

    // Description: read the avatar award entry passed in and return it
    AvatarAwardData readAvatarAwardEntry(XdbfEntry entry);

    // Description: get the URl of the image of the award passed in, hosted on xbox.com
    static string getAwardImageURL(AvatarAwardData *award, bool little);

    // Description: read in all the stuff that needs to be read at the start
    void init();

    // Description: get the next award index, for the id of a new entry
    WORD getNextAwardIndex();
};


