#pragma once

#include "AvatarAssetDefinintions.h"
#include <QString>

class AssetHelpers
{
public:
    static QString AssetSubcategoryToString(AssetSubcategory category);

    static QString AssetGenderToString(AssetGender gender);

    static QString BinaryAssetTypeToString(BinaryAssetType type);

    static QString SkeletonVersionToString(SkeletonVersion version);

    static QString BlockIDToString(STRRBBlockId id);
};
