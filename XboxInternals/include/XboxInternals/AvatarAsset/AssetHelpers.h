#pragma once

#include <iostream>
#include <XboxInternals/AvatarAsset/AvatarAssetDefinitions.h>

#include <XboxInternals/Export.h>

using std::string;

class XBOXINTERNALS_EXPORT AssetHelpers
{
public:
    static string AssetSubcategoryToString(AssetSubcategory category);

    static string AssetGenderToString(AssetGender gender);

    static string BinaryAssetTypeToString(BinaryAssetType type);

    static string SkeletonVersionToString(SkeletonVersion version);

    static string BlockIDToString(STRRBBlockId id);
};



