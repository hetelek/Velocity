#pragma once

#include "XDBF.h"
#include "XDBFDefininitions.h"
#include "XDBFHelpers.h"
#include "GPDBase.h"
#include <iostream>

using std::string;

class AvatarAwardGPD : public GPDBase
{
public:
    AvatarAwardGPD(string gpdPath);
    AvatarAwardGPD(FileIO *io);
    ~AvatarAwardGPD(void);

    vector<struct AvatarAward> avatarAwards;

    // Description: get the gender of the avatar award passed in
    static AssetGender GetAssetGender(struct AvatarAward *award);

    // Description: unlock all of the avatar awards in this gpd
    void UnlockAllAwards();

    // Description: get the URL of the image of the award passed in, hosted on xbox.com, 64x64
    static string GetLittleAwardImageURL(struct AvatarAward *award);

    // Description: get the URL of the image of the award passed in, hosted on xbox.com, 128x128
    static string GetLargeAwardImageURL(struct AvatarAward *award);

    // Description: get the GUID of the avatar award, used in URLs on xbox.com
    static string GetGUID(struct AvatarAward *award);

    // Description: write the avatar award back to the gpd
    void WriteAvatarAward(struct AvatarAward *award);

    // Description: create a new avatar award
    void CreateAvatarAward(struct AvatarAward *award);

    // Description: delete an award
    void DeleteAvatarAward(struct AvatarAward *award);

    // Description: remove all of the unused memory from the gpd
    void CleanGPD();

private:

    // Description: read the avatar award entry passed in and return it
    struct AvatarAward readAvatarAwardEntry(XDBFEntry entry);

    // Description: get the URl of the image of the award passed in, hosted on xbox.com
    static string getAwardImageURL(struct AvatarAward *award, bool little);

    // Description: read in all the stuff that needs to be read at the start
    void init();

    // Description: get the next award index, for the id of a new entry
    WORD getNextAwardIndex();
};
